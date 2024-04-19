//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <appbase/application.hpp>

#include "utils.h"
#include "Cleaner.h"
#include "Writer.h"
#include "WebsocketServer.h"

namespace bpo = boost::program_options;

class Relic_plugin : public appbase::plugin<Relic_plugin>
{
public:
	Relic_plugin() {};

	~Relic_plugin() {};

	APPBASE_PLUGIN_REQUIRES();

	virtual void set_program_options(bpo::options_description& cli, bpo::options_description& cfg) override
	{
		cfg.add_options()
			("listen-endpoint", bpo::value<std::string>()->default_value("127.0.0.1:9876"), "The local IP address and port to listen for incoming connections.")
			("remote-endpoint", bpo::value< std::vector<std::string> >()->composing(), "The IP address and port of a remote peer to sync with.")
			("public-endpoint", bpo::value<std::string>()->default_value("0.0.0.0:9876"), "The public IP address and port that should be advertized to peers.")
			;
	}

	void plugin_initialize(const boost::program_options::variables_map& options)
	{
	}

	void plugin_startup()
	{
		StdOut(Info, "Starting Relic...");
	}

	void plugin_shutdown()
	{
		StdOut(Info, "Shutdown Relic...");
	}
};

int main(int argc, char** argv)
{
	std::set_terminate([]() { STDOUT_CURRENT_EXCEPTION(NULL); std::abort(); });

	StdOut(LogLevel::Info, "VERSION: %s %s", APP_NAME, APP_VERSION);
	/*if (argc < 2)
	{
		StdOut(Info, "USAGE:\r\n\r\nOptions:\r\n\r\n-c : cleaner;\r\n-w : writer;");
		return 0;
	}*/

	try
	{
		//std::string s = std::string(argv[1]);
		//if (s == "-c")
		/*{
			Cleaner cleaner = Cleaner();
			cleaner.Run();
		}*/
		//else if (s == "-w")
		{
			Writer writer = Writer();
			writer.Run();
		}
		appbase::app().register_plugin<Relic_plugin>();
		if (!appbase::app().initialize<Relic_plugin>(argc, argv))
			return -1;
		//initialize_logging();
		appbase::app().startup();
		appbase::app().exec();
		StdOut(Info, "Exiting cleanly...");
	}
	catch (const boost::exception& e)
	{
		StdOut(Error, boost::diagnostic_information(e));
	}
	catch (Exception e)
	{
		e.StdOut();
	}
	catch (const std::exception& e)
	{
		StdOut(Error, e.what()/*boost::diagnostic_information(e)*/);
	}
	catch (...)
	{
		STDOUT_CURRENT_EXCEPTION(NULL);
	}

	getchar();
	return 0;
}