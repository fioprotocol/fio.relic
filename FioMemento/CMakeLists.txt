
cmake_minimum_required (VERSION 3.12)

project ("FioMemento" VERSION 0.0.1)

add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

#add_compile_definitions(StreamTimeTolerableShift=0.000001)#seconds

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

find_package(
    mariadbcpp
)
#include_directories("/usr/include/mariadb") #!!!change to find_package
add_library(mariadbcpp STATIC IMPORTED)
set_property(TARGET mariadbcpp PROPERTY IMPORTED_LOCATION "/usr/lib/libmariadbcpp.so") #path to libmariadbcpp.so

#pkg_check_modules(mariadb REQUIRED IMPORTED_TARGET "mysqlclient")

#target_link_libraries(${EXECUTABLE} PUBLIC  
    #Boost::filesystem
    #Boost::optional
#)
#target_include_directories(${EXECUTABLE} PRIVATE include)

#add_subdirectory(externals/appbase EXCLUDE_FROM_ALL)

#set(CMAKE_CXX_STANDARD 17)

# set(FioMemento_SOURCES
	# "main.cpp" 	 
	# "cJSON.c" "cJSON.h" 
	# "cJSON_Utils.c" "cJSON_Utils.h" 
	# "utils.cpp"	"utils.h" 		
 #	"StreamProcessor.cpp" "StreamProcessor.h"
# )

add_executable (FioMemento 
	"main.cpp" 	 
	#"cJSON.c" "cJSON.h" 
	#"cJSON_Utils.c" "cJSON_Utils.h" 
	"utils.cpp"	"utils.h"
	"SocketServer.cpp" "SocketServer.h"
	"Database.h" "Database.cpp"
)

#target_link_libraries(FioMemento 
	#${CHRONICLE_RECEIVER_NAME}
#	PRIVATE appbase
#)

#set(HEADER_FILES ${YOUR_DIRECTORY}/file1.h ${YOUR_DIRECTORY}/file2.h)
#add_library(mylib libsrc.cpp ${HEADER_FILES})
#target_include_directories(mylib PRIVATE ${YOUR_DIRECTORY})
#add_executable(myexec execfile.cpp ${HEADER_FILES})
#target_include_directories(myexec PRIVATE ${YOUR_DIRECTORY})