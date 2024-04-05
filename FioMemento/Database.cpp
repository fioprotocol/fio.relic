//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

//https://mariadb.com/docs/server/connect/programming-languages/cpp/sample-app/
//https://mariadb.com/docs/server/connect/programming-languages/cpp/development/

#include <mariadb/conncpp.hpp>
//#include <conncpp.hpp>

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

		connection = std::unique_ptr<sql::Connection>(driver->connect(url, properties));

		sth_get_min_irrev = std::unique_ptr<sql::PreparedStatement>(connection->prepareStatement("SELECT MIN(irreversible) FROM SYNC"));
		sth_get_min_tx_block = std::unique_ptr<sql::PreparedStatement>(connection->prepareStatement("SELECT MIN(block_num) FROM TRANSACTIONS"));
		sth_prune_transactions = std::unique_ptr<sql::PreparedStatement>(connection->prepareStatement("DELETE FROM TRANSACTIONS WHERE block_num < ?"));
		sth_prune_receipts = std::unique_ptr<sql::PreparedStatement>(connection->prepareStatement("DELETE FROM RECEIPTS WHERE block_num < ?"));
	}
	catch (sql::SQLException& e) 
	{
		std::cerr << "MariaDB error: " << e.what() << std::endl;
	}
}

void Database::Close()
{
	if (connection)
	{
		connection->close();
		connection = NULL;
	}
}

void Database::Prune(int blockNumber)
{
	try 
	{
		sth_prune_transactions->setInt(1, blockNumber);
		sth_prune_transactions->executeQuery();
		sth_prune_receipts->setInt(1, blockNumber);
		sth_prune_receipts->executeQuery();
	}
	catch (sql::SQLException& e)
	{
		std::cerr << "Error updating task status: " << e.what() << std::endl;
	}
}