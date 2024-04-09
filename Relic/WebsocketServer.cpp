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
	websocket::stream<beast::tcp_stream> Websocket;
	beast::flat_buffer Buffer;

public:

	explicit Session(tcp::socket&& socket) : Websocket(std::move(socket))// Take ownership of the socket
	{
	}

	void Run()// Get on the correct executor
	{
		// We need to be executing within a strand to perform async operations
		// on the I/O objects in this session. Although not strictly necessary
		// for single-threaded contexts, this example code is written to be
		// thread-safe by default.
		net::dispatch(Websocket.get_executor(), beast::bind_front_handler(&Session::OnRun, shared_from_this()));
	}

	void OnRun()// Start the asynchronous operation
	{
		// Set suggested timeout settings for the websocket
		Websocket.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
		// Set a decorator to change the Server of the handshake
		Websocket.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async"); }));
		// Accept the websocket handshake
		Websocket.async_accept(beast::bind_front_handler(&Session::OnAccept, shared_from_this()));
	}

	void OnAccept(beast::error_code ec)
	{
		if (ec)
			THROW_SocketException2("accept", ec);
		Read();
	}

	void Read()
	{
		for (;;)
			Websocket.async_read(Buffer, beast::bind_front_handler(&Session::OnRead, shared_from_this()));
	}

	void OnRead(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if (ec == websocket::error::closed)
			return;

		if (ec)
			THROW_SocketException2("accept", ec);

		//Websocket.text(Websocket.got_text());		
		//Websocket.async_write(Buffer.data(), beast::bind_front_handler(&Session::on_write, shared_from_this()));// Echo the message
	}

	void Write()
	{
		Websocket.text(Websocket.got_text());
		Websocket.async_write(Buffer.data(), beast::bind_front_handler(&Session::OnWrite, shared_from_this()));
	}

	void OnWrite(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			THROW_SocketException2("write", ec);

		Buffer.consume(Buffer.size());
	}
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
	}

	void Run()
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

		for (;;)//!!!blocking
			acceptor.async_accept(net::make_strand(ioc), beast::bind_front_handler(&Listener::onAccept, shared_from_this()));// The new connection gets its own strand
	}

private:

	void onAccept(beast::error_code ec, tcp::socket socket)
	{
		if (ec)
			THROW_SocketException2("accept", ec);
		std::make_shared<Session>(std::move(socket))->Run();// blocked till disconnect
	}

	void acceptAsync()
	{
		acceptor.async_accept(net::make_strand(ioc), beast::bind_front_handler(&Listener::onAcceptAsync, shared_from_this()));
	}

	void onAcceptAsync(beast::error_code ec, tcp::socket socket)
	{
		//onAccept(ec, socket);
		if (ec)
			THROW_SocketException2("accept", ec);
		std::make_shared<Session>(std::move(socket))->Run();// blocked till disconnect
		acceptAsync();
	}

	net::io_context& ioc;
	tcp::endpoint endpoint;
	tcp::acceptor acceptor;
};

void WebsocketServer::RunAsync(int threadNumber)
{
	threadNumber = std::max<int>(1, threadNumber);
	net::io_context ioc(threadNumber);// The io_context is required for all I/O

	std::make_shared<Listener>(ioc, endpoint)->Run();

	threads.reserve(threadNumber);
	for (int i = threadNumber; i > 0; --i)// Run the I/O service on the requested number of threads
	{
		std::thread* t = new std::thread([&ioc]
			{
				ioc.run();
			});
		threads.push_back(t);
	}
}

void WebsocketServer::Run(int threadNumber)
{
	threadNumber = std::max<int>(1, threadNumber);
	net::io_context ioc(threadNumber);// The io_context is required for all I/O

	std::make_shared<Listener>(ioc, endpoint)->Run();

	threadNumber -= 1;
	threads.reserve(threadNumber);
	for (int i = threadNumber; i > 0; --i)// Run the I/O service on the requested number of threads
	{
		std::thread* t = new std::thread([&ioc]
			{
				ioc.run();
			});
		threads.push_back(t);
	}
	ioc.run();
}

void WebsocketServer::Close()
{
	/*for (int i = threads.size()-1; i >= 0; --i)
	{
		std::thread* t = threads[i];
		t->a
	}*/
}

