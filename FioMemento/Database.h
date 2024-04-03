//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef Database_H
#define Database_H

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

	Database();

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

	//bool ProcessPacket(AVPacket* packet);
	//void Flush();
	void Close();

	enum Status Status()
	{
		return status;
	}

protected:
	enum Status status = Status::Error;
};

#endif //Database_H
