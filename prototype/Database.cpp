//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdio.h>
#include <string.h>

#include "Database.h"

void Database::Initialize(std::string user, std::string password, std::string url)
{
	/*Database::user = std::string(user);
	Database::password = std::string(password);
	Database::url = std::string(url);*/

	//try
	//{
		sql::Driver* driver = sql::mariadb::get_driver_instance();

		sql::Properties properties({ {"user", user}, {"password", password} });

		connection = driver->connect(url, properties);
	//}
	//catch (sql::SQLException& e)
	//{
	//	//THROW_Exception2("Database error: %s", ((sql::SQLException)e).getMessage().c_str());
	//	THROW_DatabaseException2(e);
	//}
}

void Database::Close()
{
	if (connection)
	{
		connection->close();
		delete(connection);
		connection = NULL;
	}
}