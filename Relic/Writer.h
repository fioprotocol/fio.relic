//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef Writer_H
#define Writer_H

#include "Database.h"
#include "utils.h"
#include "WebsocketServer.h"
#include "rapidjson/document.h"

class Writer :public WebsocketServer,public Database
{
public:

	Writer() :WebsocketServer()
	{
	}

	~Writer()
	{
		try
		{
			Close();
		}
		catch (...)
		{
			STDOUT_CURRENT_EXCEPTION("Close()");
		}
	}

	void Close();
	void Run();

protected:

	void onRead(const beast::flat_buffer& buffer) override; 
	void onDisconnect() override;

private:
	void sanityCheck();
	int processData(int msgType, rapidjson::Document& json, char* jsonStr);

	sql::PreparedStatement* sth_upd_sync_head = NULL;
	sql::PreparedStatement* sth_fork_bkp = NULL;
	sql::PreparedStatement* sth_upd_sync_fork = NULL;
	sql::PreparedStatement* sth_check_sync_health = NULL;
	sql::PreparedStatement* sth_am_i_master = NULL;
	sql::PreparedStatement* sth_clean_bkp = NULL;
	sql::PreparedStatement* sth_prune_transactions = NULL;
	sql::PreparedStatement* sth_prune_receipts = NULL;
	sql::PreparedStatement* sth_fetch_forking_traces = NULL;
	sql::PreparedStatement* sth_fork_receipts = NULL;
	sql::PreparedStatement* sth_fork_transactions = NULL;

	bool iAmMaster = false;
	bool justCommitted = false;
	int sourceId = 1;
	bool noTraces = false;
	bool keepBlocks = false;
	time_t retiredTime = 0;
	int logId = -1;
};

#endif //Writer_H
