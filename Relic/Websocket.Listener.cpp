//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include "Websocket.Listener.h"

using namespace Websocket;

void Listener::Close()
{
	acceptor.close();
	for (auto s : sessions)
		s->Close();
	sessions.empty();
}

void Listener::Run()
{
	beast::error_code ec;

	acceptor.open(endpoint.protocol(), ec);
	if (ec)
		THROW_SocketException2("open", ec);

	acceptor.set_option(net::socket_base::reuse_address(true), ec);
	if (ec)
		THROW_SocketException2("set_option", ec);

	acceptor.bind(endpoint, ec);
	if (ec)
		THROW_SocketException2("bind", ec);

	acceptor.listen(net::socket_base::max_listen_connections, ec);
	if (ec)
		THROW_SocketException2("listen", ec);

	acceptor.async_accept(net::make_strand(ioc), beast::bind_front_handler(&Listener::onAccept, shared_from_this()));// The new connection gets its own strand
}

void Listener::onAccept(beast::error_code ec, tcp::socket socket)
{
	if (ec)
		THROW_SocketException2("accept", ec);

	Session* session = newSession(std::move(socket)); //newSession(&socket);// new Session(std::move(socket));
	sessions.push_back(session);
	session->Run();//(!) blocking till disconnect
	auto s = std::find(sessions.begin(), sessions.end(), session);
	if (s != sessions.end())
	{
		sessions.erase(s);
		delete session;
	}

	acceptor.async_accept(net::make_strand(ioc), beast::bind_front_handler(&Listener::onAccept, shared_from_this()));// The new connection gets its own strand
}

