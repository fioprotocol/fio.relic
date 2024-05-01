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

class WebsocketServer //: public std::enable_shared_from_this<WebsocketServer>
{
public:

	WebsocketServer()// :sp(this)
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

	void Run(int port, std::string ip = ""/*INADDR_ANY*/);
	void RunAsync(int port, std::string ip = ""/*INADDR_ANY*/);
	void Close();
	void Write(const boost::asio::const_buffer& buffer);

	bool Binary = true;
	bool AutoFragment = true;
	void onAccept(boost::system::error_code ec, tcp::socket socket);

protected:
	virtual	void onRead(const beast::flat_buffer& buffer) = 0;
	virtual	void onDisconnect() = 0;

private:
	//std::shared_ptr<WebsocketServer> sp;//required for shared_from_this()

	void initialize(int port, std::string ip); 
	void acceptAsync();
	void read(tcp::socket&& socket);

	net::io_context ioc;// The io_context is required for all I/O
	//net::executor_work_guard<net::io_context::executor_type> work = make_work_guard(ioc.get_executor());//https://stackoverflow.com/a/69609876/23553997
	websocket::stream<beast::tcp_stream>* websocket = NULL;
	tcp::acceptor* acceptor = NULL;
};

#endif //SocketServer_H
