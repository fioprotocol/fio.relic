cmake_minimum_required (VERSION 3.12)

project ("Relic" VERSION 0.0.1)

add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

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

set(BOOST_MIN_VERSION 1.71.0) 
find_package(
    Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS ${BOOST_REQUIRED_COMPONENTS} #/usr/include/boost
)

#include_directories("/home/cliver/appbase/include") #for appbase sources
#include_directories("c:/Users/serge/Documents/_PROJECTS/fio-relic/Relic/externals/appbase/include") #for appbase sources

find_package(
    mariadbcpp #/usr/include/mariadb
)

#https://stackoverflow.com/questions/72590567/use-mariadb-connector-cpp-with-cmake-project
#add_subdirectory("/usr/include/mariadb")
#add_library(mariadbcpp SHARED IMPORTED)
#set_property(TARGET mariadbcpp PROPERTY IMPORTED_LOCATION "/usr/lib/libmariadbcpp.so") #path to libmariadbcpp.so

add_executable (${PROJECT_NAME} 
	"main.cpp" 	 
	"utils.cpp"	"utils.h"
	"Websocket.Session.cpp" "Websocket.Session.h"
	"Websocket.Listener.cpp" "Websocket.Listener.h"
	"Websocket.Server.cpp" "Websocket.Server.h"
	#"WebsocketServer.cpp" "WebsocketServer.h"
	"Database.h" "Database.cpp"
	"Cleaner.cpp" "Cleaner.h"
	"Writer.cpp" "Writer.h"
	"WriterWebsocketSession.cpp" "WriterWebsocketSession.h"
)

#target_include_directories(${PROJECT_NAME} PUBLIC 
#	${Boost_INCLUDE_DIRS} #/usr/include/
#	"/usr/include/boost"
#) 

target_link_libraries(${PROJECT_NAME} PUBLIC 
	${Boost_LIBRARIES}
	mariadbcpp	#/usr/lib/libmariadbcpp.so
)
#message(Boost_LIBRARIES="${Boost_LIBRARIES}")
#message(Boost_INCLUDE_DIRS="${Boost_INCLUDE_DIRS}")