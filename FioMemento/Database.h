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

class Database
{
	/*enum Status
	{
		Open,
		Connected,
		Error
	};*/

public:

	Database(char* user = "memento_rw", char* password = "LKpoiinjdscudfc", const char* url = "jdbc:mariadb://localhost:3306/FioMemento")
	{
		Database::user = std::string(user);
		Database::password = std::string(password);
		Database::url = std::string(url);
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

	void Initialize();
	void Close();

	/*enum Status Status()
	{
		return status;
	}*/

	sql::Connection* Connection = NULL;

protected:
	//enum Status status = Status::Error;
	std::string user;
	std::string password;
	std::string url;
};

#endif //Database_H
