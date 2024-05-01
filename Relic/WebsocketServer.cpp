//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include "WebsocketServer.h"

#define THROW_SocketException2(action, ec) throw Exception2(__FILE__, __LINE__, __FUNCTION__, "Websocket::%s: %s", action, ec.message().c_str())

void WebsocketServer::initialize(int port, std::string ip)
{
	tcp::endpoint endpoint(tcp::v4(), port);
	if (!ip.empty() && ip != std::to_string(INADDR_ANY))
		endpoint.address(net::ip::make_address(ip));
	endpoint.port(port);

	StdOut(Info, "Starting websocket server...");

	beast::error_code ec;

	acceptor = new tcp::acceptor(ioc);
	acceptor->open(endpoint.protocol(), ec);
	if (ec)
		THROW_SocketException2("open", ec);

	acceptor->set_option(net::socket_base::reuse_address(true), ec);
	if (ec)
		THROW_SocketException2("set_option", ec);

	acceptor->bind(endpoint, ec);
	if (ec)
		THROW_SocketException2("bind", ec);

	acceptor->listen(net::socket_base::max_listen_connections, ec);
	if (ec)
		THROW_SocketException2("listen", ec);
}

void WebsocketServer::RunAsync(int port, std::string ip)
{
	initialize(port, ip);
	acceptAsync();
}

void WebsocketServer::acceptAsync()
{
	THROW_Exception2("!!!FIX ME");
	ioc.reset();
	ioc.run();//it seems that a thread is required because it blocks

	acceptor->async_accept(
		beast::bind_front_handler(
			&WebsocketServer::onAccept,
			this//shared_from_this()
		)
	);

	StdOut(Info, "Waiting for connection...");
}

void WebsocketServer::onAccept(boost::system::error_code ec, tcp::socket socket)
{
	if (ec)
		THROW_SocketException2("accept", ec);

	read(std::move(socket));
}

void WebsocketServer::read(tcp::socket&& socket)
{
	StdOut(Info, "Connected");

	websocket = new websocket::stream<beast::tcp_stream>(std::move(socket));
	websocket->binary(Binary);
	websocket->auto_fragment(AutoFragment);
	websocket->set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
	//websocket->set_option(websocket::stream_base::decorator([](websocket::response_type& res) {res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async"); }));
	StdOut(Info, "Handshaking...");
	beast::error_code ec;
	websocket->accept(ec);
	if (ec)
		THROW_SocketException2("handshake", ec);
	//StdOut(Info, "Handshaked.");
	for (;;)
	{
		beast::flat_buffer buffer;
		websocket->read(buffer, ec);
		if (ec)
		{
			onDisconnect();
			StdOut(Info, "Connection closed: %s", ec.message().c_str());
			acceptAsync();
			break;
		}
		onRead(buffer);
	}
}

void WebsocketServer::Run(int port, std::string ip)
{
	initialize(port, ip);

	beast::error_code ec;

	for (;;)
	{
		StdOut(Info, "Waiting for connection...");
		tcp::socket socket = acceptor->accept(ec);

		if (ec)
			THROW_SocketException2("accept", ec);

		read(std::move(socket));
	}
}

void WebsocketServer::Close()
{
	beast::error_code ec;
	if (acceptor)
		acceptor->close(ec);
	if (ec)
		THROW_SocketException2("close", ec);
	if (websocket)
		websocket->close(websocket::close_code::going_away, ec);
	if (ec)
		THROW_SocketException2("close", ec);
}

void WebsocketServer::Write(const boost::asio::const_buffer& buffer)
{
	beast::error_code ec;
	if (!websocket)
		THROW_Exception2("No connection to write in.");
	websocket->text(websocket->got_text());
	websocket->write(buffer, ec);
	if (ec)
		THROW_SocketException2("write", ec);
}

