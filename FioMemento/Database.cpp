//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

//https://mariadb.com/docs/server/connect/programming-languages/cpp/sample-app/
//https://mariadb.com/docs/server/connect/programming-languages/cpp/development/

//#include <mariadb/conncpp.hpp>

#include <stdio.h>
#include <string.h>

#include "Database.h"

void Database::Initialize()
{
	try
	{
		sql::Driver* driver = sql::mariadb::get_driver_instance();

		sql::SQLString url(Database::url);
		sql::Properties properties({ {"user", Database::user}, {"password", Database::password} });

		connection = driver->connect(url, properties);

		sth_get_min_irrev = connection->prepareStatement("SELECT MIN(irreversible) FROM SYNC");
		sth_get_min_tx_block = connection->prepareStatement("SELECT MIN(block_num) FROM TRANSACTIONS");
		sth_prune_transactions = connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num < ?");
		sth_prune_receipts = connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num < ?");
	}
	catch (sql::SQLException& e)
	{
		std::cerr << "MariaDB error: " << ((sql::SQLException)e).getMessage() << std::endl;
		throw e;
	}
}

void Database::Close()
{
	if (connection)
	{
		/*sth_get_min_irrev->close();
		sth_get_min_tx_block->close();
		sth_prune_transactions->close();
		sth_prune_receipts->close();*/
		connection->close();
		connection = NULL;
	}
}

void Database::Prune(int blockNumber)
{
	sth_prune_transactions->setInt(1, blockNumber);
	sth_prune_transactions->executeQuery();
	sth_prune_receipts->setInt(1, blockNumber);
	sth_prune_receipts->executeQuery();
}