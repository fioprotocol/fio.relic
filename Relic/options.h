//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef Options_H
#define Options_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#define CONFIG_FILE "config.ini"

static constexpr const char* OPT_help = "help";
static constexpr const char* OPT_runWriter = "writer";
static constexpr const char* OPT_runCleaner = "cleaner";
 
static constexpr const char* OPT_sourceId = "source-id";
static constexpr const char* OPT_websocketServerIp = "ip";
static constexpr const char* OPT_websocketServerPort = "port";
static constexpr const char* OPT_dbUser = "db-user";
static constexpr const char* OPT_dbPassword = "db-password";
static constexpr const char* OPT_dbUrl = "db-url";
static constexpr const char* OPT_ackEvery = "ack-every";
static constexpr const char* OPT_keepDays = "keep-days";
static constexpr const char* OPT_noTraces = "no-traces";

void StdOutConfiguration(po::variables_map vm);

#endif //Options_H