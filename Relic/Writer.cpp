//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>

#include "Writer.h"
#include "WebsocketServer.h"

void Writer::Run()
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

	WebsocketServer::Run();
}

void Writer::onRead(const beast::flat_buffer& buffer)
{
	/*auto s = beast::buffers_to_string(buffer.data());
	std::fprintf(stdout, s.c_str());
	fflush(stdout);*/
	StdOut(Info, "%s", beast::buffers_to_string(buffer.data()).c_str());
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
	r->next();
	int mastersNumber = r->rowsCount();
	if (mastersNumber < 1)
		THROW_Exception2("No master is defined in SYNC table");
	if (mastersNumber > 1)
		THROW_Exception2("More than one master is defined in SYNC table");
	int masterSourceId = r->getInt(0);
	int masterBlockNumber = r->getInt(1);
	bool masterIrreversible = r->getInt(2);

	// sanity check, there cannot be more than one slave
	sth = connection->prepareStatement("SELECT sourceid, block_num, irreversible FROM SYNC WHERE is_master=0");
	r = sth->executeQuery();
	int slavesNumber = r->rowsCount();
	if (slavesNumber > 1)
		THROW_Exception2("SYNC table contains more than one slave");

	// fetch last sync status
	sth = connection->prepareStatement("SELECT block_num, irreversible, is_master FROM SYNC WHERE sourceid=?");
	sth_prune_transactions->setInt(1, sourceId);
	r = sth->executeQuery();
	r->next();
	if (r->isAfterLast())
		THROW_Exception2("sourceid=%s is not defined in SYNC table", sourceId);

	int confirmedBlock = r->getInt(0);
	int unconfirmedBlock = confirmedBlock;
	int irreversible = r->getInt(1);
	bool iAmMaster = r->getInt(2);
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