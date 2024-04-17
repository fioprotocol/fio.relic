cmake_minimum_required (VERSION 3.12)

project ("Relic" VERSION 0.0.1)

add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#set(PLATFORM_SPECIFIC_LIBS pthread)

set(BOOST_REQUIRED_COMPONENTS
	date_time 
	filesystem 
	system
	chrono
	program_options
	unit_test_framework 
    #thread 
	#iostreams 
)

find_package(
    Boost 1.71.0 REQUIRED COMPONENTS ${BOOST_REQUIRED_COMPONENTS} #/usr/include/boost
)

find_package(
    mariadbcpp #/usr/include/mariadb
)

#add_subdirectory(externals/appbase#(!)make sure to remove add_subdirectory( examples ) from externals/appbase/CMakeLists.txt
#EXCLUDE_FROM_ALL)
#find_package( appbase )#if pre-installed
add_library(appbase 
	"externals/appbase/application.cpp"
	"externals/appbase/include/appbase/application.hpp"
	"externals/appbase/include/appbase/version.hpp"
	"externals/appbase/include/appbase/plugin.hpp"
	"externals/appbase/include/appbase/method.hpp"
)

add_executable (${PROJECT_NAME} 
	"main.cpp" 	 
	"utils.cpp"	"utils.h"
	"Websocket.Session.cpp" "Websocket.Session.h"
	"Websocket.Listener.cpp" "Websocket.Listener.h"
	"Websocket.Server.cpp" "Websocket.Server.h"
	"WebsocketServer.cpp" "WebsocketServer.h"
	"Database.h" "Database.cpp"
	"Cleaner.cpp" "Cleaner.h"
	"Writer.cpp" "Writer.h"
)

include_directories(
  externals/appbase/include
  externals/rapidjson/include
  ${Boost_INCLUDE_DIR}
) 

target_link_libraries(${PROJECT_NAME}
	${Boost_LIBRARIES}
	appbase
	mariadbcpp	#/usr/lib/libmariadbcpp.so
	pthread #???required by appbase
)

#message(Boost_LIBRARIES="${Boost_LIBRARIES}")
#message(Boost_INCLUDE_DIRS="${Boost_INCLUDE_DIRS}")