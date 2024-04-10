//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>

#include "Database.h"

void Database::Initialize(char* user, char* password, const char* url)
{
	Database::user = std::string(user);
	Database::password = std::string(password);
	Database::url = std::string(url);

	try
	{
		sql::Driver* driver = sql::mariadb::get_driver_instance();

		sql::SQLString url(Database::url);
		sql::Properties properties({ {"user", Database::user}, {"password", Database::password} });

		connection = driver->connect(url, properties);

		if (!connection->getAutoCommit())
			THROW_Exception2("Autocommit is not enabled.");
	}
	catch (sql::SQLException& e)
	{
		THROW_Exception2("Database error: %s", ((sql::SQLException)e).getMessage().c_str());
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