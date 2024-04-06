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

//#include "externals/appbase/include/appbase/application.hpp"

#include "utils.h"
#include "Cleaner.h"

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
		{
			Cleaner cleaner = Cleaner();
			cleaner.Run();
		}
		//else if (s == "-w")
		{
			//Cleaner cleaner = Cleaner();
			//cleaner.Run();
		}
	}
	catch (Exception e)
	{
		e.StdOut();
	}
	catch (...)
	{
		STDOUT_CURRENT_EXCEPTION(NULL);
	}

	//try {
	//	appbase::app().register_plugin<net_plugin>();
	//	if (!appbase::app().initialize<net_plugin>(argc, argv))
	//		return -1;
	//	//initialize_logging();
	//	appbase::app().startup();
	//	appbase::app().exec();
	//}
	//catch (const boost::exception& e) {
	//	std::cerr << boost::diagnostic_information(e) << "\n";
	//}
	//catch (const std::exception& e) {
	//	std::cerr << e.what() << "\n";
	//}
	//catch (...) {
	//	std::cerr << "unknown exception\n";
	//}
	//std::cout << "exited cleanly\n";
	return 0;
}

//class net_plugin : public appbase::plugin<net_plugin>
//{
//public:
//	net_plugin() {};
//	~net_plugin() {};
//
//	//APPBASE_PLUGIN_REQUIRES((chain_plugin));
//
//	/*virtual void set_program_options(options_description& cli, options_description& cfg) override
//	{
//		cfg.add_options()
//			("listen-endpoint", bpo::value<string>()->default_value("127.0.0.1:9876"), "The local IP address and port to listen for incoming connections.")
//			("remote-endpoint", bpo::value< vector<string> >()->composing(), "The IP address and port of a remote peer to sync with.")
//			("public-endpoint", bpo::value<string>()->default_value("0.0.0.0:9876"), "The public IP address and port that should be advertized to peers.")
//			;
//	}*/
//
//	void plugin_initialize(const variables_map& options) { std::cout << "initialize net plugin\n"; }
//	void plugin_startup() { std::cout << "starting net plugin \n"; }
//	void plugin_shutdown() { std::cout << "shutdown net plugin \n"; }
//};
