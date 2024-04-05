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

//#include <appbase/application.hpp>

//extern "C" {
//#include "cJSON.h"
//}
//
#include "utils.h"

int main(int argc, char** argv)
{
	std::set_terminate([]() { STDOUT_CURRENT_EXCEPTION(NULL); std::abort(); });

	StdOut(LogLevel::Info, "VERSION: %s %s", APP_NAME, APP_VERSION);
	//StdOut(LogLevel::Info, "TEST: %s %s", "test1", "APP_VERSION");
	if (argc < 2)
	{
		//StdOutUsage();
		return 0;
	}

	try
	{
	}
	catch (Exception e)
	{
		e.StdOut();
	}
	catch (...)
	{
		STDOUT_CURRENT_EXCEPTION(NULL);
	}

	/*try {
		appbase::app().register_plugin<receiver_plugin>();	
		if (!appbase::app().initialize<receiver_plugin>(argc, argv))
			return -1;
		initialize_logging();
		appbase::app().startup();
		appbase::app().exec();
	}
	catch (const boost::exception& e) {
		std::cerr << boost::diagnostic_information(e) << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
	}
	catch (...) {
		std::cerr << "unknown exception\n";
	}
	std::cout << "exited cleanly\n";
	return 0;*/
}
