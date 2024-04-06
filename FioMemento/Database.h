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
	enum Status
	{
		Open,
		Connected,
		Error
	};

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

	void Prune(int blockNumber);

	enum Status Status()
	{
		return status;
	}

protected:
	enum Status status = Status::Error;
	sql::Connection* connection = NULL;
	std::string user;
	std::string password;
	std::string url;

	sql::PreparedStatement* sth_get_min_irrev = NULL;
	sql::PreparedStatement* sth_get_min_tx_block = NULL;
	sql::PreparedStatement* sth_prune_transactions = NULL;
	sql::PreparedStatement* sth_prune_receipts = NULL;
};

#endif //Database_H
