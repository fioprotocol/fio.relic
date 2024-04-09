//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include "Websocket.Server.h"

using namespace Websocket;

void Server::run(int threadNumber, bool async)
{
	threadNumber = std::max<int>(1, threadNumber);
	net::io_context ioc(threadNumber);// The io_context is required for all I/O

	listener = std::make_shared<Listener>(ioc, endpoint);
	listener->Run();

	if (!async)
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
	if (!async)
		ioc.run();//!!!blocking
}

void Server::RunAsync(int threadNumber)
{
	run(threadNumber, true);
}

void Server::Run(int threadNumber)
{
	run(threadNumber, false);
}

void Server::Close()
{
	if (listener)
	{
		listener->Close();
		listener = NULL;
	}
	/*for (int i = threads.size()-1; i >= 0; --i)
	{
		std::thread* t = threads[i];
		t->a
	}*/
}

