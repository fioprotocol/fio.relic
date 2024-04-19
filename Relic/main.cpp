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
#include <boost/exception/diagnostic_information.hpp> 
#include <appbase/application.hpp>

#include "utils.h"
#include "Cleaner.h"
#include "Writer.h"
#include "options.h"

namespace po = boost::program_options;

class Relic_plugin : public appbase::plugin<Relic_plugin>
{
public:
	Relic_plugin() {};

	~Relic_plugin() {};

	APPBASE_PLUGIN_REQUIRES();

	virtual void set_program_options(po::options_description& cli, po::options_description& cfg) override
	{
	}

	void plugin_initialize(const boost::program_options::variables_map& options)
	{
		StdOut(Info, "Starting Relic...");
	}

	void plugin_startup()
	{
		//appbase::app().quit();
	}

	void plugin_shutdown()
	{
		StdOut(Info, "Shutdown Relic...");
	}

private:
};

int main(int argc, char** argv)
{
	std::set_terminate(
		[]()
		{
			STDOUT_CURRENT_EXCEPTION(NULL);
	std::abort();
		}
	);

	try
	{
		StdOut(LogLevel::Info, "VERSION: %s %s", APP_NAME, APP_VERSION);

		po::options_description od("Main options");
		od.add_options()
			((std::string(OPT_help) + ",?").c_str(), "Relic usage description.")
			((std::string(OPT_runWriter) + ",w").c_str(), "Run writer. (!)It is the default option.")
			((std::string(OPT_runCleaner) + ",c").c_str(), "Run cleaner.")
			;
		po::variables_map vm;
		store(po::command_line_parser(argc, argv).options(od).allow_unregistered().run(), vm);
		if (vm.count(OPT_help))
		{
			std::cout << "USAGE:\r\nOptions can be defined either as command line arguments or in " << CONFIG_FILE << "\r\n";
			od.print(std::cout);
			//std::cout << "\r\nWriter options:\r\n\r\n";
			Writer::GetOptionsDescription().print(std::cout);
			//std::cout << "\r\Cleaner options:\r\n\r\n";
			Cleaner::GetOptionsDescription().print(std::cout);
			return 0;
		}
		if (vm.count(OPT_runCleaner))
		{
			if (vm.count(OPT_runWriter))
				throw Exception("Options --%s and --%s cannot be specified together.", OPT_runCleaner, OPT_runWriter);
			Cleaner cleaner(argc, argv);
			cleaner.Run();
		}
		else
		{
			Writer writer(argc, argv);
			writer.Run();
		}

		/*appbase::app().register_plugin<Relic_plugin>();
		if (!appbase::app().initialize<Relic_plugin>(argc, argv))
			return -1;
		appbase::app().startup();
		appbase::app().exec();*/

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

	return 0;
}