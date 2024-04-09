//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef WebsocketSession_H
#define WebsocketSession_H

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

#define THROW_SocketException2(...) throw Exception2(__FILE__, __LINE__, __FUNCTION__, "Websocket: %s (code: %i)", ## __VA_ARGS__)

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Websocket
{
	class Session : public std::enable_shared_from_this<Session>
	{
	public:

		explicit Session(tcp::socket&& socket) : websocket(std::move(socket))// Take ownership of the socket
		{
		}

		~Session()
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

		void Run();

		void Close();

		void Write(beast::flat_buffer buffer);

		std::function<void(beast::flat_buffer buffer)> OnRead;

	private:
		websocket::stream<beast::tcp_stream> websocket;

		void onRun();
		void onAccept(beast::error_code ec);

		void read();
		void onRead(beast::error_code ec, std::size_t bytes_transferred);
		beast::flat_buffer bufferIn;

		void onWrite(beast::error_code ec, std::size_t bytes_transferred);
		beast::flat_buffer* bufferOut;
	};
}

#endif //WebsocketSession_H
