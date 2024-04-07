cmake_minimum_required (VERSION 3.12)

project ("FioMemento" VERSION 0.0.1)

add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

#include(GNUInstallDirs)

set(BOOST_REQUIRED_COMPONENTS
	date_time 
	filesystem 
	system
	chrono
	program_options
	unit_test_framework 
    #thread 
	#iostreams 

	#charconv #MySql
)

set(BOOST_MIN_VERSION 1.71.0) 
find_package(
    Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS ${BOOST_REQUIRED_COMPONENTS}
)

include_directories("/home/cliver/appbase/include") #for appbase sources
include_directories("c:/Users/serge/Documents/_PROJECTS/fio-relic/FioMemento/externals/appbase/include") #for appbase sources
include_directories("/usr/include/boost")

find_package(
    mariadbcpp
)
#include_directories("/usr/include/mariadb") #!!!change to find_package

#https://stackoverflow.com/questions/72590567/use-mariadb-connector-cpp-with-cmake-project
#add_subdirectory("/usr/include/mariadb")
#add_library(mariadbcpp SHARED IMPORTED)
#set_property(TARGET mariadbcpp PROPERTY IMPORTED_LOCATION "/usr/lib/libmariadbcpp.so") #path to libmariadbcpp.so

add_executable (${PROJECT_NAME} 
	"main.cpp" 	 
	#"cJSON.c" "cJSON.h" 
	#"cJSON_Utils.c" "cJSON_Utils.h" 
	"utils.cpp"	"utils.h"
	"SocketServer.cpp" "SocketServer.h"
	"Database.h" "Database.cpp"
	#${SOURCES_FILES_LIBS}
	"Cleaner.cpp" "Cleaner.h"
	#"c:/Users/serge/Documents/_PROJECTS/fio-relic/FioMemento/externals/appbase/application.cpp"
)

target_link_libraries(${PROJECT_NAME} "/usr/lib/libmariadbcpp.so")