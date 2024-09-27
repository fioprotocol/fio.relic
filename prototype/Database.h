//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef Database_H
#define Database_H

#include <mariadb/conncpp.hpp>

#include "utils.h"

#define THROW_DatabaseException2(exception) throw Exception2(__FILE__, __LINE__, __FUNCTION__, "Database: %s", ((sql::SQLException)exception).getMessage().c_str())

class Database
{
	/*enum Status
	{
		Open,
		Connected,
		Error
	};*/

public:

	Database()
	{
	}

	~Database()
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

	//void Initialize(const char* user = "memento_rw", const char* password = "LKpoiinjdscudfc", const char* url = "jdbc:mariadb://localhost:3306/Relic");
	void Initialize(std::string user, std::string password, std::string url);
	void Close();

	/*enum Status Status()
	{
		return status;
	}*/


protected:
	//enum Status status = Status::Error;
	/*std::string user;
	std::string password;
	std::string url;*/
	sql::Connection* connection = NULL;
};

#endif //Database_H
