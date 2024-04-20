//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <boost/exception/diagnostic_information.hpp> 

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/json.hpp>
//#include <boost/json/src.hpp>

#include "Writer.h"
#include "WebsocketServer.h"
#include "options.h"

po::options_description Writer::GetOptionsDescription()
{
	po::options_description od("Writer options");
	od.add_options()
		//((std::string(OPT_help) + ",?").c_str(), "Relic usage description.")
		(OPT_sourceId, po::value<int>()->default_value(1), "Relic writer ID.")
		(OPT_websocketServerPort, po::value<int>()->default_value(8800), "Relic websocket port where Chronicle is to connect to.")
		(OPT_websocketServerIp, po::value<std::string>()->default_value(std::to_string(INADDR_ANY)), "Relic websocket IP where Chronicle is to connect to.")
		(OPT_dbUser, po::value<std::string>()->required(), "Relic database user.")
		(OPT_dbPassword, po::value<std::string>()->required(), "Relic database password.")
		(OPT_dbUrl, po::value<std::string>()->required(), "Relic database url.")
		(OPT_ackEvery, po::value<int>()->default_value(100), "Send acknowledgements every N blocks.")
		(OPT_keepDays, po::value<int>()->default_value(-1), "Delete the history older tnan N days.")
		(OPT_noTraces, po::value<bool>()->default_value(false), "Skip writing TRANSACTIONS, RECEIPTS tables.")
		;
	return od;
}

bool Writer::getOptions()
{
	try
	{
		po::options_description od = GetOptionsDescription();
		po::variables_map vm;
		store(po::command_line_parser(argc, argv).options(od).allow_unregistered().run(), vm);
		store(po::parse_config_file(CONFIG_FILE, od, true), vm);
		po::notify(vm);
		sourceId = vm.at(OPT_sourceId).as<int>();
		websocketServerPort = vm.at(OPT_websocketServerPort).as<int>();
		websocketServerIp = vm.at(OPT_websocketServerIp).as<std::string>();
		dbUser = vm.at(OPT_dbUser).as<std::string>();
		dbPassword = vm.at(OPT_dbPassword).as<std::string>();
		dbUrl = vm.at(OPT_dbUrl).as<std::string>();
		ackEvery = vm.at(OPT_ackEvery).as<int>();
		int keepDays = vm.at(OPT_keepDays).as<int>();
		if (keepDays >= 0)
			keepBlocks = keepDays * 24 * 7200;
		noTraces = vm.at(OPT_noTraces).as<bool>();

		//StdOut(Info, "Configuration: "
		//	"\r\n%s=%d"
		//	"\r\n%s=%d"
		//	"\r\n%s=%s"
		//	"\r\n%s=%s"
		//	//"\r\n%s=%s"
		//	"\r\n%s=%s"
		//	"\r\n%s=%d"
		//	"\r\n%s=%d"
		//	"\r\n%s=%d",
		//	OPT_sourceId, sourceId,
		//	OPT_websocketServerPort, websocketServerPort,
		//	OPT_websocketServerIp, websocketServerIp.c_str(),
		//	OPT_dbUser, dbUser.c_str(),
		//	//OPT_dbPassword, dbPassword.c_str(),
		//	OPT_dbUrl, dbUrl.c_str(),
		//	OPT_ackEvery, ackEvery,
		//	OPT_keepDays, keepDays,
		//	OPT_noTraces, noTraces
		//);
		StdOutConfiguration(vm);
		return true;
	}
	catch (const boost::exception& e)
	{
		StdOut(Error, boost::diagnostic_information(e));
	}
	catch (const std::exception& e)
	{
		StdOut(Error, boost::diagnostic_information(e));
	}
	Writer::GetOptionsDescription().print(std::cout);
	return false;
}

void Writer::sanityCheck()
{
	// sanity check, there should be only one master
	auto sth = connection->prepareStatement("SELECT sourceid, block_num, irreversible FROM SYNC WHERE is_master = 1");
	sql::ResultSet* r = sth->executeQuery();
	int mastersNumber = r->rowsCount();
	if (mastersNumber < 1)
		THROW_Exception2("No master is defined in SYNC table");
	if (mastersNumber > 1)
		THROW_Exception2("More than one master is defined in SYNC table");
	r->first();
	int masterSourceId = r->getInt(1);
	int masterBlockNumber = r->getInt(2);
	bool masterIrreversible = r->getInt(3);

	// sanity check, there cannot be more than one slave
	sth = connection->prepareStatement("SELECT sourceid, block_num, irreversible FROM SYNC WHERE is_master=0");
	r = sth->executeQuery();
	int slavesNumber = r->rowsCount();
	if (slavesNumber > 1)
		THROW_Exception2("SYNC table contains more than one slave");

	// fetch last sync status
	sth = connection->prepareStatement("SELECT block_num, irreversible, is_master FROM SYNC WHERE sourceid=?");
	sth->setInt(1, sourceId);
	r = sth->executeQuery();
	r->next();
	if (r->isAfterLast())
		THROW_Exception2("sourceid=%s is not defined in SYNC table", sourceId);
	confirmedBlock = r->getInt64(1);
	unconfirmedBlock = confirmedBlock;
	irreversible = r->getInt64(2);
	iAmMaster = r->getInt(3);
	StdOut(Info, "Starting from confirmed_block=%d, irreversible=%d, sourceid=%d, is_master=%d", confirmedBlock, irreversible, sourceId, iAmMaster);

	if (!iAmMaster)
	{
		// make sure the master is running
		if (!masterBlockNumber || !masterIrreversible)
			THROW_Exception2("sourceid=%d is defined as master, but it has not started yet.", masterSourceId);

		if (keepBlocks >= 0)
			StdOut(Info, "Automatically pruning the history older than %d blocks", keepBlocks);
	}
}

void Writer::Run()
{
	try
	{
		if (!getOptions())
			return;

		Database::Initialize(dbUser, dbPassword, dbUrl);

		connection->setAutoCommit(false);

		sth_upd_sync_head = connection->prepareStatement("UPDATE SYNC SET block_num=?, block_time=?, block_id=?, irreversible=?, last_updated=NOW() WHERE sourceid=?");
		sth_fork_bkp = connection->prepareStatement("DELETE FROM BKP_TRACES WHERE block_num>=?");
		sth_upd_sync_fork = connection->prepareStatement("UPDATE SYNC SET block_num=? WHERE sourceid=?");
		sth_check_sync_health = connection->prepareStatement("SELECT sourceid, irreversible, is_master, TIME_TO_SEC(TIMEDIFF(NOW(), last_updated)) AS upd FROM SYNC");
		sth_am_i_master = connection->prepareStatement("SELECT is_master FROM SYNC WHERE sourceid=?");
		sth_clean_bkp = connection->prepareStatement("DELETE FROM BKP_TRACES WHERE block_num < (SELECT MIN(irreversible) FROM SYNC)");
		sth_prune_transactions = connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num < ?");
		sth_prune_receipts = connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num < ?");
		sth_fetch_forking_traces = connection->prepareStatement("SELECT seq, block_num, block_time, trx_id, trace FROM TRANSACTIONS WHERRE block_num >= ? ORDER BY seq DESC");
		sth_fork_receipts = connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num>=?");
		sth_fork_transactions = connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num>=?");
		sth_clean_log = connection->prepareStatement("DELETE FROM EVENT_LOG WHERE block_num < (SELECT MIN(irreversible) FROM SYNC) - ?");
		sth_upd_sync1 = connection->prepareStatement("UPDATE SYNC SET is_master=0 WHERE sourceid != ?");
		sth_upd_sync2 = connection->prepareStatement("UPDATE SYNC SET is_master=1 WHERE sourceid = ?");
		sth_fetch_bkp_traces = connection->prepareStatement("SELECT seq, block_num, block_time, trx_id, trace FROM BKP_TRACES WHERE block_num >= ? ORDER BY seq");
		sth_insrt_bkp_traces = connection->prepareStatement("INSERT IGNORE INTO BKP_TRACES (seq, block_num, block_time, trx_id, trace) VALUES (?,?,?,?,?)");
		sth_insert_transactions = connection->prepareStatement("INSERT IGNORE INTO TRANSACTIONS (seq, block_num, block_time, trx_id, trace) VALUES (?,?,?,?,?)");
		sth_insert_receipts = connection->prepareStatement("INSERT IGNORE INTO RECEIPTS (seq, block_num, block_time, contract, action, receiver, recv_sequence) VALUES (?,?,?,?,?,?,?)");
		sth_insert_recv_seq_max = connection->prepareStatement("INSERT INTO RECV_SEQUENCE_MAX (account_name, recv_sequence_max) VALUES (?,?) ON DUPLICATE KEY UPDATE recv_sequence_max = VALUES(recv_sequence_max)");

		//if (keepDays > 0)
		//	keepBlocks = keepDays * 24 * 7200;

		sanityCheck();

		if (noTraces)
			StdOut(Info, "Skipping the updates for TRANSACTIONS, RECEIPTS, RECV_SEQUENCE_MAX tables.");

		connection->commit();
	}
	catch (sql::SQLException& e)
	{
		THROW_DatabaseException2(e);
	}

	WebsocketServer::Run(websocketServerPort, websocketServerIp);
}

uint32_t readInt32_LittleEndian(char* cs)
{
	//auto i = (cs[3] << 24) | (cs[2] << 16) | (cs[1] << 8) | cs[0];
	//auto i = (cs[0] << 24) | (cs[1] << 16) | (cs[2] << 8) | cs[3];
	//cs += 4;
	//return i;
	uint32_t n;
	memcpy(&n, cs, sizeof(n));
	return n;
}

//uint32_t readInt32_LittleEndian(std::string& s)
//{
//	uint32_t n;
//	memcpy(&n, s.c_str(), sizeof(n));
//	s = s.erase(0, 4);
//	return n;
//}

void Writer::onRead(const beast::flat_buffer& buffer)
{
	if (iAmMaster && justCommitted)
	{
		// verify that I am still the master
		sth_am_i_master->setInt(1, sourceId);
		auto r = sth_am_i_master->executeQuery();
		if (!r->first() || !r->getByte(1))
		{
			StdOut(Warning, "I am no longer the master (sourceid=%d)", sourceId);
			iAmMaster = false;
			retiredTime = std::chrono::system_clock::now();
			logId = -1;
		}
		justCommitted = false;
	}

	int ack = processData(buffer);
	if (ack >= 0)
	{
		std::string s = std::to_string(ack);
		const boost::asio::const_buffer b(s.data(), s.size());
		Write(b);
		StdOut(Info, "ack %d", ack);
	}
}

void Writer::onDisconnect()
{
	connection->rollback();
}

void Writer::Close()
{
	WebsocketServer::Close();

	delete sth_upd_sync_head;
	delete sth_fork_bkp;
	delete sth_upd_sync_fork;
	delete sth_check_sync_health;
	delete sth_am_i_master;
	delete sth_clean_bkp;
	delete sth_prune_transactions;
	delete sth_prune_receipts;
	delete sth_fetch_forking_traces;
	delete sth_fork_receipts;
	delete sth_fork_transactions;
	delete sth_clean_log;
	delete sth_upd_sync1;
	delete sth_upd_sync2;
	delete sth_fetch_bkp_traces;
	delete sth_insrt_bkp_traces;
	delete sth_insert_transactions;
	delete sth_insert_receipts;
	delete sth_insert_recv_seq_max;

	Database::Close();
}

std::string getBlockTime(rapidjson::Document& data)
{
	auto bt = std::string(data["block_timestamp"].GetString());
	return bt.replace(bt.find("T"), 1, " ");
}

int Writer::processData(const beast::flat_buffer& buffer)
{
	if (!connection->isValid() && !connection->reconnect())
		THROW_Exception2("Could not reconnect the db.");

	std::string buffer_ = beast::buffers_to_string(buffer.data());
	uint msgType = readInt32_LittleEndian(&buffer_[0]);
	uint opts = readInt32_LittleEndian(&buffer_[4]);
	std::string jsonStr = &buffer_[8];
	rapidjson::Document json;
	json.Parse(jsonStr.c_str());
	if (json.HasParseError())
		THROW_Exception2("JSON error: %d, offset: %d", json.GetParseError(), json.GetErrorOffset());

	StdOut(Info, "msgType: %d", msgType);
	switch (msgType)
	{
	case 1001: // CHRONICLE_MSGTYPE_FORK
	{
		int64_t blockNum = std::stol(json["block_num"].GetString());
		StdOut(Info, "Fork at %d", blockNum);

		connection->commit();
		justCommitted = true;

		if (confirmedBlock <= irreversible)
			return blockNum - 1;

		confirmedBlock = blockNum - 1;
		unconfirmedBlock = confirmedBlock;

		insertTransactions.clear();
		insertReceipts.clear();
		upsertRecvSeqMax.clear();

		if (iAmMaster)
			forkTraces(blockNum);
		else
		{
			sth_fork_bkp->setInt64(1, blockNum);
			sth_fork_bkp->execute();
		}

		sth_upd_sync_fork->setInt64(1, confirmedBlock);
		sth_upd_sync_fork->setInt(2, sourceId);
		sth_upd_sync_fork->execute();
		connection->commit();
		justCommitted = true;

		return confirmedBlock;
	}
	case 1003: // CHRONICLE_MSGTYPE_TX_TRACE
	{
		int64_t blockNum = std::stol(json["block_num"].GetString());
		if (blockNum <= confirmedBlock)
			return -1;

		rapidjson::GenericObject<false, rapidjson::Value> trace = json["trace"].GetObject();
		if (strcasecmp(trace["status"].GetString(), "executed"))
			return -1;
		auto actionTraces = trace["action_traces"].GetArray();
		if (!actionTraces.Size())
			return -1;

		auto blockTime = getBlockTime(json);
		uint64_t trxSeq = std::stoul(actionTraces[0]["receipt"]["global_sequence"].GetString());
		if (iAmMaster)
			saveTrace(trxSeq, blockNum, std::move(blockTime), trace, std::move(jsonStr));
		else
			// push(@insert_bkp_traces,	[$trx_seq, $block_num, $dbh->quote($block_time), $dbh->quote($trace->{'id'}),$dbh->quote(${ $jsptr }, $db_binary_type)]);
			insertBkpTraces.push_back(Trace{ trxSeq, blockNum, std::move(blockTime), std::move(trace["id"].GetString()), std::move(jsonStr) });

		trxCounter++;

		return -1;
	}
	case 1010: // CHRONICLE_MSGTYPE_BLOCK_COMPLETED
	{
		blocksCounter++;
		auto blockTime = getBlockTime(json);
		int64_t lastIrreversible = std::stol(json["last_irreversible"].GetString());

		int64_t blockNum = std::stol(json["block_num"].GetString());
		if (blockNum > unconfirmedBlock + 1)
			StdOut(Warning, "Missing blocks %d to %d", unconfirmedBlock + 1, blockNum - 1);

		if (blockNum > lastIrreversible)
			ackEvery = 1;

		if (lastIrreversible > irreversible)
		{
			// LIB has moved
			irreversible = lastIrreversible;
			/*foreach my $hook(@lib_hooks)
			{
				& {$hook}($irreversible);
			}*/

			if (!iAmMaster)
			{
				sth_clean_bkp->execute();
				if (keepBlocks >= 0)
				{
					long int upto = irreversible - keepBlocks;
					sth_prune_receipts->setInt(1, upto);
					sth_prune_receipts->execute();
					sth_prune_transactions->setInt(1, upto);
					sth_prune_transactions->execute();
					/*	foreach my $hook(@prune_hooks)
						{
							& {$hook}($upto);
						}*/
				}
			}
		}

		if (iAmMaster)
		{
			sendTracesBatch();//!!!it may be non committed???
			/*foreach my $hook(@block_hooks)
			{
				& {$hook}(blockNum, lastIrreversible, $data->{'block_id'});
			}*/
		}
		else if (insertBkpTraces.size())
		{
			for (Trace t : insertBkpTraces)
			{
				sth_insrt_bkp_traces->setUInt64(1, t.seq);
				sth_insrt_bkp_traces->setInt64(2, t.block_num);
				sth_insrt_bkp_traces->setString(3, t.block_time);
				sth_insrt_bkp_traces->setString(4, t.trx_id);
				sth_insrt_bkp_traces->setString(5, t.trace);
				sth_insrt_bkp_traces->addBatch();
			}
			sth_insrt_bkp_traces->executeBatch();
			insertBkpTraces.clear();
		}

		unconfirmedBlock = blockNum;

		if (unconfirmedBlock <= confirmedBlock)
			// we are catching up through irreversible data, and this block was already stored in DB
			return unconfirmedBlock;//!!!shouldn't we call rollback() here???

		if (unconfirmedBlock - confirmedBlock < ackEvery)
			return -1;

		/*if (iAmMaster)
		foreach my $hook(@ack_hooks)
		& {$hook}(blockNum);*/

		sth_upd_sync_head->setInt(1, blockNum);
		sth_upd_sync_head->setString(2, blockTime);
		sth_upd_sync_head->setString(3, json["block_id"].GetString());
		sth_upd_sync_head->setInt64(4, lastIrreversible);
		sth_upd_sync_head->setInt(5, sourceId);
		connection->commit();
		justCommitted = true;
		confirmedBlock = unconfirmedBlock;

		if (!iAmMaster && blockNum > lastIrreversible && std::chrono::system_clock::now() > retiredTime + std::chrono::seconds(60))
		{
			// check if the master is still alive
			int my_upd = -1;
			long int my_irrev = -1;
			int master_upd = -1;
			long int master_irrev = -1;
			int old_master = -1;

			auto r = sth_check_sync_health->executeQuery();
			for (r->first(); r->isAfterLast(); r->next())
			{
				int my_sourceid = r->getInt("sourceid");
				if (my_sourceid == sourceId)
				{
					my_upd = r->getInt("upd");
					my_irrev = r->getInt64("irreversible");
				}
				else if (r->getByte("is_master"))
				{
					master_upd = r->getInt("upd");
					master_irrev = r->getInt64("irreversible");
					old_master = my_sourceid;
				}
			}

			if (my_upd < 0 || my_irrev < 0 || master_upd < 0 || master_irrev < 0 || old_master < 0)
				THROW_Exception2("SYNC corrupted");

			if (master_irrev < my_irrev - 120 && master_upd > my_upd + 120 && my_upd < 10)
			{
				StdOut(Info, "Master process (sourceid=%i) stopped, taking over the master role.", old_master);
				StdOut(Info, "my_upd=%d, my_irrev=%d, master_upd=%d, master_irrev=%d", my_upd, my_irrev, master_upd, master_irrev);

				sth_upd_sync1->setInt(1, sourceId);
				sth_upd_sync1->execute();
				sth_upd_sync2->setInt(1, sourceId);
				sth_upd_sync2->execute();
				connection->commit();

				iAmMaster = true;

				StdOut(Info, "Sleeping 5 seconds...");
				sleep(5);

				// delete all reversible traces written by old master
				long startBlock = master_irrev + 1;
				forkTraces(startBlock);

				// copy data from BKP_TRACES
				int copied_rows = 0;
				sth_fetch_bkp_traces->setInt64(1, startBlock);
				r = sth_fetch_bkp_traces->executeQuery();
				for (r->first(); r->isAfterLast(); r->next())
				{
					auto bkpTraceB = r->getBlob(5);
					std::istreambuf_iterator<char> isb(*bkpTraceB);
					std::string bkpTraceJsonStr(isb, std::istreambuf_iterator<char>());
					rapidjson::Document bkpTraceJson;
					bkpTraceJson.Parse(bkpTraceJsonStr.c_str());
					saveTrace(r->getUInt64(1), r->getInt64(2), std::move(r->getString(3).c_str()), bkpTraceJson["trace"].GetObject(), std::move(bkpTraceJsonStr));
					copied_rows++;
				}

				sendTracesBatch();
				connection->commit();
				justCommitted = true;
				StdOut(Info, "Copied %d rows from backup", copied_rows);

				sth_fork_bkp->setInt64(1, startBlock);
				sth_fork_bkp->execute();
				connection->commit();
				justCommitted = true;
			}
		}

		std::tm tm = {};
		strptime(json["block_timestamp"].GetString(), "%Y-%m-%dT%H:%M:%S", &tm);
		auto blockTimestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
		float gap = std::chrono::duration<float>(std::chrono::system_clock::now() - blockTimestamp).count() / 3600;
		float period = std::chrono::duration<float>(std::chrono::system_clock::now() - counterStart).count();
		StdOut(Info, "%s - blocks/s: %8.2f, trx/block: %8.2f, trx/s: %8.2f, gap: %8.4fh", (iAmMaster ? "Master" : "Slave"), blocksCounter / period, trxCounter / blocksCounter, trxCounter / period, gap);
		counterStart = std::chrono::system_clock::now();
		blocksCounter = 0;
		trxCounter = 0;

		return confirmedBlock;
	}
	default:
	{
		//StdOut(Warning, "Unknown message type: %d", msgType);
		return -1;
	}
	}
	return -1;
}

void Writer::forkTraces(int64_t startBlock)
{
	/*foreach my $hook(@fork_hooks)
	{
		& {$hook}(startBlock);
	}*/

	/*if (scalar(@rollback_hooks) > 0)
	{
		$db->{'sth_fetch_forking_traces'}->execute(startBlock);
		while (my $r = $db->{'sth_fetch_forking_traces'}->fetchrow_arrayref())
		{
			foreach my $hook(@rollback_hooks)
			{
				& {$hook}(@{$r});
			}
		}
	}*/

	sth_fork_receipts->setInt64(1, startBlock);
	sth_fork_receipts->execute();
	sth_fork_transactions->setInt64(1, startBlock);
	sth_fork_transactions->execute();
}

void Writer::saveTrace(uint64_t trxSeq, int64_t blockNum, std::string&& blockTime, const rapidjson::GenericObject<false, rapidjson::Value>& trace, std::string&& jsonStr)
{
	if (!noTraces)
	{
		//my $qtime = $dbh->quote($block_time);
		// [$trx_seq, $block_num, $qtime, $dbh->quote($trace->{'id'}), $dbh->quote(${$jsptr}, $db_binary_type)]);!!!DBI::SQL_BINARY
		insertTransactions.push_back(Transaction{ trxSeq, blockNum, blockTime, std::move(trace["id"].GetString()), std::move(jsonStr) });
		auto actionTraces = trace["action_traces"].GetArray();
		for (rapidjson::Value::ConstValueIterator itr = actionTraces.Begin(); itr != actionTraces.End(); ++itr)
		{
			auto atrace = itr->GetObject();
			auto act = atrace["act"].GetObject();
			auto receipt = atrace["receipt"].GetObject();
			auto receiver = receipt["receiver"].GetString();
			int64_t recv_sequence = std::stol(receipt["recv_sequence"].GetString());
			//push(@insert_receipts, [$trx_seq, $block_num, $qtime, $dbh->quote($act->{'account'}),$dbh->quote($act->{'name'}), $dbh->quote($receiver),$recv_sequence]);
			insertReceipts.push_back(Receipt{ trxSeq, blockNum, blockTime, std::move(act["account"].GetString()), std::move(act["name"].GetString()), std::move(receiver), recv_sequence });
			upsertRecvSeqMax[receiver] = recv_sequence;
		}
	}

	/*foreach my $hook(@trace_hooks)
	{
		& {$hook}($trx_seq, $block_num, $block_time, $trace, $jsptr);
	}*/
}

void Writer::sendTracesBatch()
{
	if (!insertTransactions.size())
		return;


	/*std::string insert_transactions("INSERT IGNORE INTO TRANSACTIONS (seq, block_num, block_time, trx_id, trace) VALUES ");
	for (auto t : insertTransactions)
	{
		insert_transactions.append("(" + std::to_string(t.seq)).append("," + std::to_string(t.block_num)).append(",'" + t.block_time + "'").append(",'" + t.trx_id + "'").append(",'" + t.trace + "')");
	}
	connection->createStatement()->execute(insert_transactions);*/
	for (auto t : insertTransactions)
	{
		//sth_insert_transactions->clearParameters();
		sth_insert_transactions->setUInt64(1, t.seq);
		sth_insert_transactions->setInt64(2, t.block_num);
		sth_insert_transactions->setDateTime(3, t.block_time);
		sth_insert_transactions->setString(4, t.trx_id);
		std::istringstream s(t.trace);
		sth_insert_transactions->setBlob(5, &s, t.trace.size());
		//sql::bytes bs(t.trace.data(), t.trace.size());
		//sth_insert_transactions->setBytes(5, &bs);//!!!sometimes segmentation fault or Dynamic exception type: std::length_error; std::exception::what: basic_string::_M_create
		sth_insert_transactions->addBatch();
	}
	sth_insert_transactions->executeBatch();
	sth_insert_transactions->clearBatch();//(!)necessary for setBlob(), otherwise the next executeBatch() sets an empty blob.

	if (insertReceipts.size())
	{
		sth_insert_receipts->clearBatch();
		for (auto r : insertReceipts)
		{
			sth_insert_receipts->setUInt64(1, r.seq);
			sth_insert_receipts->setInt64(2, r.block_num);
			sth_insert_receipts->setDateTime(3, r.block_time);
			sth_insert_receipts->setString(4, r.contract);
			sth_insert_receipts->setString(5, r.action);
			sth_insert_receipts->setString(6, r.receiver);
			sth_insert_receipts->setInt64(7, r.recv_sequence);
			sth_insert_receipts->addBatch();
		}
		sth_insert_receipts->executeBatch();
		sth_insert_receipts->clearBatch();
	}

	if (upsertRecvSeqMax.size())
	{
		for (auto p : upsertRecvSeqMax)
		{
			sth_insert_recv_seq_max->setString(1, p.first);
			sth_insert_recv_seq_max->setInt64(2, p.second);
			sth_insert_recv_seq_max->addBatch();
		}
		sth_insert_recv_seq_max->executeBatch();
		sth_insert_recv_seq_max->clearBatch();
	}

	insertTransactions.clear();
	insertReceipts.clear();
	upsertRecvSeqMax.clear();
}


//void Writer::sendEventsBatch()
//{
//	if ($i_am_master and defined($keep_event_log) and scalar(@insert_event_log) > 0)
//	{
//		if (not defined($log_id))
//		{
//			my $sth = $db->{'dbh'}->prepare('SELECT MAX(id) FROM EVENT_LOG');
//			$sth->execute();
//			my $r = $sth->fetchall_arrayref();
//			if (scalar(@{$r}) > 0)
//			{
//				$log_id = $r->[0][0];
//			}
//			else
//			{
//				$log_id = 0;
//			}
//		}
//
//		my $query = 'INSERT INTO EVENT_LOG (id, block_num, event_type, data) VALUES ' .
//			join(',', map {'('.join(',', ++$log_id, @{$_}) . ')'} @insert_event_log);
//
//		$db->{'dbh'}->do($query);
//		@insert_event_log = ();
//	}
//}