//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

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

		Connection = driver->connect(url, properties);

		if (!Connection->getAutoCommit())
			THROW_Exception2("Autocommit is not enabled.");
	}
	catch (sql::SQLException& e)
	{
		std::cerr << "MariaDB error: " << ((sql::SQLException)e).getMessage() << std::endl;
		throw e;
	}
}

void Database::Close()
{
	if (Connection)
	{
		Connection->close();
		Connection = NULL;
	}
}