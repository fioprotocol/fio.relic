//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include "Websocket.Session.h"

using namespace Websocket;

void Session::Run()
{
	// We need to be executing within a strand to perform async operations
	// on the I/O objects in this session. Although not strictly necessary
	// for single-threaded contexts, this example code is written to be
	// thread-safe by default.
	net::dispatch(websocket.get_executor(), beast::bind_front_handler(&Session::onRun, shared_from_this()));
}

void Session::Close()
{
	beast::error_code ec;
	websocket.close(websocket::close_code::going_away, ec);
}

void Session::Write(beast::flat_buffer buffer)
{
	bufferOut = &buffer;
	websocket.text(websocket.got_text());
	websocket.async_write(buffer.data(), beast::bind_front_handler(&Session::onWrite, shared_from_this()));
}

void Session::onRun()// Start the asynchronous operation
{
	// Set suggested timeout settings for the websocket
	websocket.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
	// Set a decorator to change the Server of the handshake
	websocket.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async"); }));
	// Accept the websocket handshake
	websocket.async_accept(beast::bind_front_handler(&Session::onAccept, shared_from_this()));
}

void Session::onAccept(beast::error_code ec)
{
	if (ec)
		THROW_SocketException2("accept", ec);
	read();
}

void Session::read()
{
	websocket.async_read(bufferIn, beast::bind_front_handler(&Session::onRead, shared_from_this()));
}

void Session::onRead(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	// This indicates that the session was closed
	if (ec == websocket::error::closed)
		return;

	if (ec)
		THROW_SocketException2("accept", ec);

	if (OnRead)
		OnRead(bufferIn);
	bufferIn.consume(bufferIn.size());

	read();
}

void Session::onWrite(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec)
		THROW_SocketException2("write", ec);

	bufferOut->consume(bufferOut->size());
}