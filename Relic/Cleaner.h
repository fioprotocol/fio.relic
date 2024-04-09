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

class Cleaner
{
public:

	Cleaner(int keepDays = 1)
	{
		Cleaner::keepDays = keepDays;
		database = new Database();
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

	void Close();
	void Run();

protected:
	Database* database = NULL;
	void initialize();

	sql::PreparedStatement* sth_get_min_irrev = NULL;
	sql::PreparedStatement* sth_get_min_tx_block = NULL;
	sql::PreparedStatement* sth_prune_transactions = NULL;
	sql::PreparedStatement* sth_prune_receipts = NULL;

	int lastIrrev = 0;
	int keepBlocks = keepDays * 24 * 7200;
	int keepDays;
};

#endif //Cleaner_H
