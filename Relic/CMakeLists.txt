cmake_minimum_required (VERSION 3.12)

project ("Relic" VERSION 0.1.3)

add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

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

#export CMAKE_PREFIX_PATH=/usr/include/mariadb
#find_package(
#    mariadbcpp #/usr/include/mariadb
#)

set(CMAKE_CXX_STANDARD 17)#(!)required by appbase

add_subdirectory(
	externals/appbase#(!)use https://github.com/AntelopeIO/appbase.git as https://github.com/fioprotocol/appbase.git does not compile
	EXCLUDE_FROM_ALL
)

add_executable (${PROJECT_NAME} 
	"main.cpp" 	 
	"utils.cpp"	"utils.h"
	"WebsocketServer.cpp" "WebsocketServer.h"
	"Database.h" "Database.cpp"
	"Cleaner.cpp" "Cleaner.h"
	"Writer.cpp" "Writer.h"
	"options.h" "options.cpp"
)

include_directories(
  #externals/appbase/include
  externals/rapidjson/include
  #${Boost_INCLUDE_DIR}
) 

target_link_libraries(${PROJECT_NAME}
PRIVATE appbase
PUBLIC ${Boost_LIBRARIES}
	mariadbcpp	#/usr/lib/libmariadbcpp.so
)
