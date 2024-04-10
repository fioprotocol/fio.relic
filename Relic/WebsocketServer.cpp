//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include "WebsocketServer.h"

#define THROW_SocketException2(...) throw Exception2(__FILE__, __LINE__, __FUNCTION__, "Websocket: %s (code: %i)", ## __VA_ARGS__)

class Session : public std::enable_shared_from_this<Session>
{
public:

	Session(tcp::socket&& socket) : websocket(std::move(socket))// Take ownership of the socket
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

	void Run()
	{
		// We need to be executing within a strand to perform async operations
		// on the I/O objects in this session. Although not strictly necessary
		// for single-threaded contexts, this example code is written to be
		// thread-safe by default.
		net::dispatch(websocket.get_executor(), beast::bind_front_handler(&Session::onRun, shared_from_this()));
	}

	std::function<void(beast::flat_buffer buffer)> OnRead;

	void Close()
	{
		beast::error_code ec;
		websocket.close(websocket::close_code::going_away, ec);
	}

	void Write(beast::flat_buffer buffer)
	{
		bufferOut = &buffer;
		websocket.text(websocket.got_text());
		websocket.async_write(buffer.data(), beast::bind_front_handler(&Session::onWrite, shared_from_this()));
	}

	void onRun()// Start the asynchronous operation
	{
		// Set suggested timeout settings for the websocket
		websocket.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
		// Set a decorator to change the Server of the handshake
		websocket.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async"); }));
		// Accept the websocket handshake
		websocket.async_accept(beast::bind_front_handler(&Session::onAccept, shared_from_this()));
	}

	void onAccept(beast::error_code ec)
	{
		if (ec)
			THROW_SocketException2("accept", ec);
		read();
	}

	void read()
	{
		websocket.async_read(bufferIn, beast::bind_front_handler(&Session::onRead, shared_from_this()));
	}

	void onRead(beast::error_code ec, std::size_t bytes_transferred)
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

	void onWrite(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			THROW_SocketException2("write", ec);

		bufferOut->consume(bufferOut->size());
	}
	beast::flat_buffer bufferIn;
	beast::flat_buffer* bufferOut;
	websocket::stream<beast::tcp_stream> websocket;
};

class Listener : public std::enable_shared_from_this<Listener>
{
public:
	Listener(net::io_context& ioc_, tcp::endpoint endpoint_) : ioc(ioc_), acceptor(ioc_), endpoint(endpoint_)
	{
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

	void Close()
	{
		acceptor.close();
		if (session)
		{
			session->Close();
			session = NULL;
		}
	}

	void Run(/*std::function<void(beast::flat_buffer buffer)> read, std::function<void(beast::flat_buffer buffer)> write*/)
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

		accept();
	}

private:

	void accept()
	{
		if (session != NULL)
			THROW_Exception2("1 session exists already!");
		acceptor.async_accept(net::make_strand(ioc), beast::bind_front_handler(&Listener::onAccept, shared_from_this()));// The new connection gets its own strand

	}

	void onAccept(beast::error_code ec, tcp::socket socket)
	{
		if (ec)
			THROW_SocketException2("accept", ec);

		session = std::make_shared<Session>(std::move(socket));
		session->Run();// blocked till disconnect
		session = NULL;

		accept();
	}

	net::io_context& ioc;
	tcp::endpoint endpoint;
	tcp::acceptor acceptor;

	std::shared_ptr<Session> session = NULL;
};

void WebsocketServer::Run(/*std::function<void(beast::flat_buffer buffer)> read, std::function<void(beast::flat_buffer buffer)> write*/)
{
	net::io_context ioc;// The io_context is required for all I/O
	auto listener = std::make_shared<Listener>(ioc, endpoint);
	listener->Run(/*std::function<void(beast::flat_buffer buffer)> read, std::function<void(beast::flat_buffer buffer)> write*/);
	ioc.run();
}

void WebsocketServer::Close()
{
	/*if (listener)
	{
		listener->Close();
		listener = NULL;
	}*/
}

