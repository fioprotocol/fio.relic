//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef WebsocketListener_H
#define WebsocketListener_H

#include "utils.h"
#include "Websocket.Session.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Websocket
{
	class Listener : public std::enable_shared_from_this<Listener>
	{
	public:
		Listener(std::function<Session*(tcp::socket&& socket)> newSession, net::io_context& ioc_, tcp::endpoint endpoint_) : ioc(ioc_), acceptor(ioc_), endpoint(endpoint_)
		{
			Listener::newSession = newSession;
		}

		~Listener()
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

		void Close();

		void Run();

	private:

		void onAccept(beast::error_code ec, tcp::socket socket);

		net::io_context& ioc;
		tcp::endpoint endpoint;
		tcp::acceptor acceptor;

		std::vector<Session*> sessions;

		std::function<Session* (tcp::socket&& socket)> newSession;
	};
}

#endif //WebsocketListener_H
