//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdint.h>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/json.hpp>
//#include <boost/json/src.hpp>

#include "Writer.h"
#include "WebsocketServer.h"

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
	confirmedBlock = r->getInt(1);
	unconfirmedBlock = confirmedBlock;
	irreversible = r->getInt(2);
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
		Database::Initialize();

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
		sth_insrt_bkp_traces = connection->prepareStatement("INSERT INTO BKP_TRACES (seq, block_num, block_time, trx_id, trace) VALUES ?,?,?,?,?");
		sth_insert_transactions = connection->prepareStatement("INSERT INTO TRANSACTIONS (seq, block_num, block_time, trx_id, trace) VALUES ?,?,?,?,?");
		sth_insert_receipts = connection->prepareStatement("INSERT INTO RECEIPTS (seq, block_num, block_time, contract, action, receiver, recv_sequence) VALUES ?,?,?,?,?,?,?");
		sth_insert_recv_seq_max = connection->prepareStatement("INSERT INTO RECV_SEQUENCE_MAX (account_name, recv_sequence_max) VALUES ?,? ON DUPLICATE KEY UPDATE recv_sequence_max = VALUES(recv_sequence_max)");

		sanityCheck();

		if (noTraces)
			StdOut(Info, "Skipping the updates for TRANSACTIONS, RECEIPTS, RECV_SEQUENCE_MAX tables.");
	}
	catch (sql::SQLException& e)
	{
		THROW_DatabaseException2(e);
	}

	WebsocketServer::Run();
}

uint32_t readInt32_LittleEndian(char*& cs)
{
	//auto i = (cs[3] << 24) | (cs[2] << 16) | (cs[1] << 8) | cs[0];
	//auto i = (cs[0] << 24) | (cs[1] << 16) | (cs[2] << 8) | cs[3];
	//cs += 4;
	//return i;
	uint32_t n;
	memcpy(&n, cs, sizeof(n));
	cs += 4;
	return n;
}

uint32_t readInt32_LittleEndian(std::string& s)
{
	uint32_t n;
	memcpy(&n, s.c_str(), sizeof(n));
	s.erase(0, 4);
	return n;
}

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
		char cs[4];
		sprintf(cs, "%d", ack);
		boost::asio::const_buffer b(cs, sizeof(cs));
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

	auto buffer_ = beast::buffers_to_string(buffer.data());
	uint msgType = readInt32_LittleEndian(buffer_);
	uint opts = readInt32_LittleEndian(buffer_);
	rapidjson::Document json;
	json.Parse(buffer_.c_str());
	if (json.HasParseError())
		THROW_Exception2("JSON error: %d, offset: %d", json.GetParseError(), json.GetErrorOffset());

	boost::shared_ptr<std::string> jsonStr = boost::make_shared<std::string>(buffer_);

	long blockNum = std::stol(json["block_num"].GetString());
	switch (msgType)
	{
	case 1001: // CHRONICLE_MSGTYPE_FORK
	{
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
			sth_fork_bkp->setInt(1, blockNum);
			sth_fork_bkp->execute();
		}

		sth_upd_sync_fork->setInt(1, confirmedBlock);
		sth_upd_sync_fork->setInt(2, sourceId);
		sth_upd_sync_fork->execute();
		connection->commit();
		justCommitted = true;

		return confirmedBlock;
	}
	case 1003: // CHRONICLE_MSGTYPE_TX_TRACE
	{
		if (blockNum <= confirmedBlock)
			return -1;

		rapidjson::GenericObject<false, rapidjson::Value> trace = json["trace"].GetObject();
		if (strcasecmp(trace["status"].GetString(), "executed"))
			return -1;
		auto actionTraces = trace["action_traces"].GetArray();
		if (!actionTraces.Size())
			return -1;

		auto blockTime = getBlockTime(json);

		ulong trxSeq = std::stoul(actionTraces[0]["receipt"]["global_sequence"].GetString());
		if (iAmMaster)
			saveTrace(trxSeq, blockNum, blockTime, trace, jsonStr);
		else
			// push(@insert_bkp_traces,	[$trx_seq, $block_num, $dbh->quote($block_time), $dbh->quote($trace->{'id'}),$dbh->quote(${ $jsptr }, $db_binary_type)]);
			insertBkpTraces.push_back(Trace{ trxSeq, blockNum, blockTime, trace["id"].GetString(), jsonStr });

		trxCounter++;

		return -1;
	}
	case 1010: // CHRONICLE_MSGTYPE_BLOCK_COMPLETED
	{
		blocksCounter++;
		auto blockTime = getBlockTime(json);
		long int lastIrreversible = std::stol(json["last_irreversible"].GetString());

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
				sth_insrt_bkp_traces->setInt64(1, t.seq);
				sth_insrt_bkp_traces->setInt64(2, t.block_num);
				sth_insrt_bkp_traces->setString(3, t.block_time);
				sth_insrt_bkp_traces->setString(4, t.trx_id);
				sth_insrt_bkp_traces->setString(5, *t.trace);
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
				auto trace = json["trace"].GetObject();
				for (r->first(); r->isAfterLast(); r->next())
				{
					auto js = r->getBlob("trace");
					std::istreambuf_iterator<char> isb(*js);
					std::string s(isb, std::istreambuf_iterator<char>());
					rapidjson::Document data;
					data.Parse(s.c_str());
					saveTrace(r->getUInt64(1), r->getInt64(2), r->getString(3).c_str(), trace, jsonStr);
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
		int period = std::chrono::duration<float>(std::chrono::system_clock::now() - counterStart).count();
		StdOut(Info, "%s - blocks/s: %8.2f, trx/block: %8.2f, trx/s: %8.2f, gap: %8.4fh, ",
			(iAmMaster ? 'M' : 'S'), blocksCounter / period, trxCounter / blocksCounter, trxCounter / period, gap
		);
		counterStart = std::chrono::system_clock::now();
		blocksCounter = 0;
		trxCounter = 0;

		return confirmedBlock;
	}
	default:
	{
		StdOut(Warning, "Unknown message type: %d", msgType);
		return -1;
	}
	}
	return -1;
}

void Writer::forkTraces(long startBlock)
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

void Writer::saveTrace(ulong trxSeq, long blockNum, std::string blockTime, const rapidjson::GenericObject<false, rapidjson::Value>& trace, boost::shared_ptr<std::string> jsonStr)
{
	if (!noTraces)
	{
		//my $qtime = $dbh->quote($block_time);
		// [$trx_seq, $block_num, $qtime, $dbh->quote($trace->{'id'}), $dbh->quote(${$jsptr}, $db_binary_type)]);!!!DBI::SQL_BINARY
		insertTransactions.push_back(Transaction{ trxSeq, blockNum, blockTime, trace["id"].GetString(), jsonStr });
		auto actionTraces = trace["action_traces"].GetArray();
		for (rapidjson::Value::ConstValueIterator itr = actionTraces.Begin(); itr != actionTraces.End(); ++itr)
		{
			auto atrace = itr->GetObject();
			auto act = atrace["act"].GetObject();
			auto receipt = atrace["receipt"].GetObject();
			auto receiver = receipt["receiver"].GetString();
			long recv_sequence = std::stol(receipt["recv_sequence"].GetString());
			//push(@insert_receipts, [$trx_seq, $block_num, $qtime, $dbh->quote($act->{'account'}),$dbh->quote($act->{'name'}), $dbh->quote($receiver),$recv_sequence]);
			insertReceipts.push_back(Receipt{ trxSeq, blockNum, blockTime, act["account"].GetString(), act["name"].GetString(), receiver, recv_sequence });
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

	for (auto t : insertTransactions)
	{
		sth_insert_transactions->setUInt64(1, t.seq);
		sth_insert_transactions->setInt64(2, t.block_num);
		sth_insert_transactions->setDateTime(3, t.block_time);
		sth_insert_transactions->setString(4, t.trx_id);
		std::stringstream s(*t.trace);
		sth_insert_transactions->setBlob(5, &s);
		sth_insert_transactions->addBatch();
	}
	sth_insert_transactions->executeBatch();

	if (insertReceipts.size())
	{
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