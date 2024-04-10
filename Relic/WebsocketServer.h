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

	WebsocketServer(std::string ip = "127.0.0.1", int port = 8800)
	{
		endpoint.address(net::ip::make_address(ip));
		endpoint.port(port);
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

	void Run(/*std::function<void(beast::flat_buffer buffer)> read, std::function<void(beast::flat_buffer buffer)> write*/);
	void Close();

private:

	tcp::endpoint endpoint;
	//std::shared_ptr<Listener> listener;
	std::function<void(beast::flat_buffer buffer)> read;
	std::function<void(beast::flat_buffer buffer)> write;
};

#endif //SocketServer_H
