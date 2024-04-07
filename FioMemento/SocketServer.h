//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef SocketServer_H
#define SocketServer_H

//#include <boost/beast/core.hpp>
//#include <boost/beast/websocket.hpp>
//#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "utils.h"

class SocketServer
{
	enum Status
	{
		Open,
		Connected,
		Error
	};

public:

	SocketServer(int port);

	~SocketServer()
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

#endif //SocketServer_H
