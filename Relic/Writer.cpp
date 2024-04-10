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
	database = new Database();
	database->Initialize();

	sth_upd_sync_head = database->Connection->prepareStatement("UPDATE SYNC SET block_num=?, block_time=?, block_id=?, irreversible=?, last_updated=NOW() WHERE sourceid=?");
	sth_fork_bkp = database->Connection->prepareStatement("DELETE FROM BKP_TRACES WHERE block_num>=?");
	sth_upd_sync_fork = database->Connection->prepareStatement("UPDATE SYNC SET block_num=? WHERE sourceid=?");
	sth_check_sync_health = database->Connection->prepareStatement("SELECT sourceid, irreversible, is_master, TIME_TO_SEC(TIMEDIFF(NOW(), last_updated)) AS upd FROM SYNC");
	sth_am_i_master = database->Connection->prepareStatement("SELECT is_master FROM SYNC WHERE sourceid=?");
	sth_clean_bkp = database->Connection->prepareStatement("DELETE FROM BKP_TRACES WHERE block_num < (SELECT MIN(irreversible) FROM SYNC)");
	sth_prune_transactions = database->Connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num < ?");
	sth_prune_receipts = database->Connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num < ?");
	sth_fetch_forking_traces = database->Connection->prepareStatement("SELECT seq, block_num, block_time, trx_id, trace FROM TRANSACTIONS WHERRE block_num >= ? ORDER BY seq DESC");
	sth_fork_receipts = database->Connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num>=?");
	sth_fork_transactions = database->Connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num>=?");

	WebsocketServer::Run();
}

void Writer::OnRead(const beast::flat_buffer& buffer)
{
	/*auto s = beast::buffers_to_string(buffer.data());
	std::fprintf(stdout, s.c_str());
	fflush(stdout);*/
	StdOut(Info, "%s", beast::buffers_to_string(buffer.data()).c_str());
}

void Writer::Close()
{
	WebsocketServer::Close();

	if (database)
	{
		database->Close();
		database = NULL;
	}
}