//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef SocketServer_H
#define SocketServer_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "utils.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class WebsocketServer : public std::enable_shared_from_this<WebsocketServer>
{
public:

	WebsocketServer()
	{
	}

	~WebsocketServer()
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

	void Run(std::string ip = "127.0.0.1", int port = 8800);
	void Close();
	void Write(beast::flat_buffer& buffer);

	bool Binary = true;
	bool AutoFragment = true;

protected:
	virtual	void onRead(const beast::flat_buffer& buffer) = 0;
	virtual	void onDisconnect() = 0;

private:

	websocket::stream<beast::tcp_stream>* websocket;
};

#endif //SocketServer_H
