//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/json.hpp>
//#include <boost/json/src.hpp>

//#include "rapidjson/stringbuffer.h"

#include "Writer.h"
#include "WebsocketServer.h"

void Writer::Run()
{
	try
	{
		Database::Initialize();

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

		sanityCheck();
	}
	catch (sql::SQLException& e)
	{
		THROW_DatabaseException2(e);
	}

	WebsocketServer::Run();
}

uint32_t readInt32_LittleEndian(char*& cs)
{
	auto i = (cs[3] << 24) | (cs[2] << 16) | (cs[1] << 8) | cs[0];
	//auto i = (cs[0] << 24) | (cs[1] << 16) | (cs[2] << 8) | cs[3];
	cs += 4;
	return i;
}

//uint32_t readInt32(std::string& s)
//{
//	auto i = (s[3] << 24) | (s[2] << 16) | (s[1] << 8) | s[0];
//	s.erase(0, 4);
//	return i;
//}

void Writer::onRead(const beast::flat_buffer& buffer)
{
	auto s = beast::buffers_to_string(buffer.data());
	char* cs = &s[0];
	uint msgType = readInt32_LittleEndian(cs);
	uint opts = readInt32_LittleEndian(cs);
	rapidjson::Document json;
	json.Parse(cs);
	if (json.HasParseError())
		THROW_Exception2("JSON error: %d, offset: %d", json.GetParseError(), json.GetErrorOffset());

	if (iAmMaster && justCommitted)
	{
		// verify that I am still the master
		sth_am_i_master->setInt(1, sourceId);
		auto r = sth_am_i_master->executeQuery();
		if (!r->first() || !r->getByte(1))
		{
			StdOut(Warning, "I am no longer the master (sourceid=%d)", sourceId);
			iAmMaster = false;
			retiredTime = time(NULL);
			logId = -1;
		}
		justCommitted = false;
	}

	int ack = processData(msgType, json, cs);
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

	int confirmedBlock = r->getInt(1);
	int unconfirmedBlock = confirmedBlock;
	int irreversible = r->getInt(2);
	iAmMaster = r->getInt(3);
	StdOut(Info, "Starting from confirmed_block=%d, irreversible=%d, sourceid=%d, is_master=%d", confirmedBlock, irreversible, sourceId, iAmMaster);

	if (noTraces)
		StdOut(Info, "Skipping the updates for TRANSACTIONS, RECEIPTS, RECV_SEQUENCE_MAX tables.");

	if (!iAmMaster)
	{
		// make sure the master is running
		if (!masterBlockNumber || !masterIrreversible)
			THROW_Exception2("sourceid=%d is defined as master, but it has not started yet.", masterSourceId);

		if (keepBlocks)
			StdOut(Info, "Automatically pruning the history older than %d blocks", keepBlocks);
	}
}

int Writer::processData(int msgType, rapidjson::Document& json, char* jsonStr)
{

}