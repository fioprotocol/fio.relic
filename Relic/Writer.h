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
#include <rapidjson/document.h>

class Writer :public WebsocketServer, public Database
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
	void Run(/*int sourceId, bool noTraces = false, int ackEvery = 100*/);

	std::string GetUsage()
	{
		return std::string("Usage: $0 --id=N --dsn=DBSTRING [options...]\n"
			"The utility opens a WS port for Chronicle to send data to.\n"
			"Options:\n"
			"  --id=N             source instance identifier (1 or 2)\n"
			"  --port=N           \[$port\] TCP port to listen to websocket connection\n"
			"  --ack=N            \[$ack_every\] Send acknowledgements every N blocks\n"
			"  --dsn=DBSTRING     database connection string\n"
			"  --dbuser=USER      \[$db_user\]\n"
			"  --dbpw=PASSWORD    \[$db_password\]\n"
			"  --keepdays=N       delete the history older tnan N days\n"
			//"  --plugin=FILE.pl   plugin program for custom processing\n"
			//"  --parg KEY=VAL     plugin configuration options\n"
			"  --notraces         skip writing TRANSACTIONS, RECEIPTS tables\n"
		);
	}

protected:

	void onRead(const beast::flat_buffer& buffer) override;
	void onDisconnect() override;

private:

	int sourceId = 1;
	bool noTraces = false;
	int ackEvery = 100;

	void sanityCheck();
	int processData(const beast::flat_buffer& buffer);
	void forkTraces(int64_t startBlock);
	void saveTrace(uint64_t trxSeq, int64_t blockNum, std::string&& blockTime, const rapidjson::GenericObject<false, rapidjson::Value>& trace, std::string&& jsonStr);
	//void sendEventsBatch();
	void sendTracesBatch();

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
	sql::PreparedStatement* sth_clean_log = NULL;
	sql::PreparedStatement* sth_upd_sync1 = NULL;
	sql::PreparedStatement* sth_upd_sync2 = NULL;
	sql::PreparedStatement* sth_fetch_bkp_traces = NULL;
	sql::PreparedStatement* sth_insrt_bkp_traces = NULL;
	sql::PreparedStatement* sth_insert_transactions = NULL;
	sql::PreparedStatement* sth_insert_receipts = NULL;
	sql::PreparedStatement* sth_insert_recv_seq_max = NULL;

	bool iAmMaster = false;
	bool justCommitted = false;
	long int keepBlocks = -1;
	std::chrono::system_clock::time_point retiredTime = {};
	int logId = -1;
	int64_t confirmedBlock = -1;
	int64_t irreversible = -1;
	int64_t unconfirmedBlock = -1;
	int trxCounter = 0;
	int blocksCounter = 0;
	std::chrono::system_clock::time_point counterStart = {};

	struct Trace
	{
		uint64_t seq;
		int64_t block_num;
		std::string block_time;
		std::string trx_id;
		std::string trace;
	};
	std::vector<Trace> insertBkpTraces;

	struct Transaction
	{
		uint64_t seq;
		int64_t block_num;
		std::string block_time;
		std::string trx_id;
		std::string trace;
	};
	std::vector<Transaction> insertTransactions;

	struct Receipt
	{
		uint64_t seq;
		int64_t block_num;
		std::string block_time;
		std::string contract;
		std::string action;
		std::string receiver;
		int64_t recv_sequence;
	};
	std::vector<Receipt> insertReceipts;

	std::map<std::string, int64_t>	upsertRecvSeqMax;

	//struct Event
	//{

	//};
	//bool keepEventLog = false;
	//std::vector<Event> insertEventLog;
};

#endif //Writer_H
