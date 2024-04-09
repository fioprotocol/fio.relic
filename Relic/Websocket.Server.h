//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef WebsocketServer_H
#define WebsocketServer_H

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
#include "Websocket.Listener.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Websocket
{
	class Server : public std::enable_shared_from_this<Server>
	{
	public:

		Server(std::function<Session*(tcp::socket&& socket)> newSession, std::string ip = "127.0.0.1", int port = 8800)
		{
			Server::newSession = newSession;
			endpoint.address(net::ip::make_address(ip));
			endpoint.port(port);
		}

		~Server()
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

		void Run(int threadNumber);
		void Close();
		void RunAsync(int threadNumber);

	private:

		void run(int threadNumber, bool async);

		std::function<Session*(tcp::socket&& socket)> newSession;

		tcp::endpoint endpoint;
		std::vector<std::thread*> threads;
		std::shared_ptr<Listener> listener;
	};
}

#endif //WebsocketServer_H
