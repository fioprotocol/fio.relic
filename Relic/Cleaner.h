//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef Cleaner_H
#define Cleaner_H

#include "Database.h"
#include "utils.h"

class Cleaner :public Database
{
public:

	Cleaner(int keepDays = 1)
	{
		Cleaner::keepDays = keepDays;
	}

	~Cleaner()
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

	std::string GetUsage()
	{
		return std::string(
			"Usage: $0 --keepdays = N --database = DBNAME[options...]\n"
			"The utility opens a WS port for Chronicle to send data to.\n"
			"Options:\n"
			"  --dsn=DBSTRING     database connection string\n"
			"  --dbuser=USER      \[$db_user\]\n"
			"  --dbpw=PASSWORD    \[$db_password\]\n"
			"  --keepdays=N       delete the history older tnan N days\n"
			//"  --plugin=FILE.pl   plugin program for custom processing\n"
			//"  --parg KEY=VAL     plugin configuration options\n"
		);
	}

	void Close();
	void Run();

protected:

	sql::PreparedStatement* sth_get_min_irrev = NULL;
	sql::PreparedStatement* sth_get_min_tx_block = NULL;
	sql::PreparedStatement* sth_prune_transactions = NULL;
	sql::PreparedStatement* sth_prune_receipts = NULL;

	int lastIrrev = 0;
	int keepBlocks = keepDays * 24 * 7200;
	int keepDays;
};

#endif //Cleaner_H
