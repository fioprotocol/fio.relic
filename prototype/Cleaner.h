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

#include <boost/program_options.hpp>
namespace po = boost::program_options;

class Cleaner :public Database
{
public:

	Cleaner(int argc, char** argv)
	{
		Cleaner::argc = argc;
		Cleaner::argv = argv;
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

	static po::options_description GetOptionsDescription();

	void Close();
	void Run();

protected:
	int argc;
	char** argv;

	std::string dbUser;
	std::string dbPassword;
	std::string dbUrl;
	//int keepDays = -1;

	bool getOptions();

	sql::PreparedStatement* sth_get_min_irrev = NULL;
	sql::PreparedStatement* sth_get_min_tx_block = NULL;
	sql::PreparedStatement* sth_prune_transactions = NULL;
	sql::PreparedStatement* sth_prune_receipts = NULL;

	int lastIrrev = 0;
	int keepBlocks = -1;
};

#endif //Cleaner_H
