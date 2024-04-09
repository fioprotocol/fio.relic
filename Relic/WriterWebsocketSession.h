//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef WriterWebsocketSession_H
#define WriterWebsocketSession_H

#include "utils.h"
#include "Websocket.Session.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


class WriterWebsocketSession :public Websocket::Session
{
public:

	WriterWebsocketSession(tcp::socket&& socket) : Websocket::Session(std::move(socket))
	{
	}

	void OnRead(beast::flat_buffer buffer) override;

private:
};

#endif //WriterWebsocketSession_H
