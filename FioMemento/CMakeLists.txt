
cmake_minimum_required (VERSION 3.12)

project ("FioMemento" VERSION 0.0.1)

add_compile_definitions(APP_NAME="${PROJECT_NAME}")
add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

#add_compile_definitions(StreamTimeTolerableShift=0.000001)#seconds

#include(GNUInstallDirs)

set(BOOST_REQUIRED_COMPONENTS
    thread 
	unit_test_framework 
	filesystem 
	date_time 
	system 
	iostreams 
	program_options
)

set(BOOST_MIN_VERSION 1.67.0) 
#find_package(
    #Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS ${BOOST_REQUIRED_COMPONENTS}
#)

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


target_link_libraries(FioMemento #${CHRONICLE_RECEIVER_NAME}
  #PRIVATE appbase
)