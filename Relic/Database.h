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

	void Initialize(char* user = "memento_rw", char* password = "LKpoiinjdscudfc", const char* url = "jdbc:mariadb://localhost:3306/Relic");
	void Close();

	/*enum Status Status()
	{
		return status;
	}*/


protected:
	//enum Status status = Status::Error;
	std::string user;
	std::string password;
	std::string url;
	sql::Connection* connection = NULL;
};

#endif //Database_H
