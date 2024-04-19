//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <boost/exception/diagnostic_information.hpp> 

#include "Cleaner.h"
#include "options.h"

po::options_description Cleaner::GetOptionsDescription()
{
	po::options_description od("Cleaner options");
	od.add_options()
		//((std::string(OPT_help) + ",?").c_str(), "Relic usage description.")
		(OPT_dbUser, po::value<std::string>()->required(), "Relic database user.")
		(OPT_dbPassword, po::value<std::string>()->required(), "Relic database password.")
		(OPT_dbUrl, po::value<std::string>()->required(), "Relic database url.")
		(OPT_keepDays, po::value<int>()->default_value(-1), "Delete the history older tnan N days.")
		;
	return od;
}

bool Cleaner::getOptions()
{
	try
	{
		po::options_description od = GetOptionsDescription();
		po::variables_map vm;
		store(po::command_line_parser(argc, argv).options(od).allow_unregistered().run(), vm);
		store(po::parse_config_file(CONFIG_FILE, od, true), vm);
		po::notify(vm);
		dbUser = vm.at(OPT_dbUser).as<std::string>();
		dbPassword = vm.at(OPT_dbPassword).as<std::string>();
		dbUrl = vm.at(OPT_dbUrl).as<std::string>();
		int keepDays = vm.at(OPT_keepDays).as<int>();
		if (keepDays >= 0)
			keepBlocks = keepDays * 24 * 7200;

		//StdOut(Info, "Configuration: "
		//	"\r\n%s=%s"
		//	//"\r\n%s=%s"
		//	"\r\n%s=%s"
		//	"\r\n%s=%d",
		//	OPT_dbUser, dbUser.c_str(),
		//	//OPT_dbPassword, dbPassword.c_str(),
		//	OPT_dbUrl, dbUrl.c_str(),
		//	OPT_keepDays, keepDays
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
		StdOut(Error, e.what()/*boost::diagnostic_information(e)*/);
	}
	Cleaner::GetOptionsDescription().print(std::cout);
	return false;
}

void Cleaner::Run()
{
	try
	{
		if (!getOptions())
			return;

		Database::Initialize();

		connection->setAutoCommit(false);

		sth_get_min_irrev = connection->prepareStatement("SELECT MIN(irreversible) FROM SYNC");
		sth_get_min_tx_block = connection->prepareStatement("SELECT MIN(block_num) FROM TRANSACTIONS");
		sth_prune_transactions = connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num < ?");
		sth_prune_receipts = connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num < ?");

		for (;; std::this_thread::sleep_for(std::chrono::seconds(10)))
		{
			if (!connection->isValid() && !connection->reconnect())
				THROW_Exception2("Could not reconnect the db.");

			sql::ResultSet* r = sth_get_min_irrev->executeQuery();
			connection->commit();
			r->next();
			//r->isAfterLast();
			int minIrrev = r->getInt(1);
			if (lastIrrev >= minIrrev)
			{
				StdOut(Warning, "Nothing to delete");
				continue;
			}

			lastIrrev = minIrrev;
			int	uptoBlock = lastIrrev - keepBlocks;
			r = sth_get_min_tx_block->executeQuery();
			r->next();
			int minBlock = r->getInt(1);
			if (minBlock < uptoBlock)
				StdOut(Error, "Pruning %d blocks", uptoBlock - minBlock);

			while (minBlock < uptoBlock)
			{
				int deleteUpto = minBlock + 10;
				if (deleteUpto > uptoBlock)
					deleteUpto = uptoBlock;

				StdOut(Error, "Deleting blocks < %d", deleteUpto);
				sth_prune_transactions->setInt(1, deleteUpto);
				sth_prune_transactions->executeQuery();
				sth_prune_receipts->setInt(1, deleteUpto);
				sth_prune_receipts->executeQuery();
				sth_prune_receipts->executeQuery();
				connection->commit();

				minBlock = deleteUpto;
			}
		}
	}
	catch (sql::SQLException& e)
	{
		THROW_DatabaseException2(e);
	}
}

void Cleaner::Close()
{
	Database::Close();
}