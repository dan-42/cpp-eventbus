#------------------------------------
# Global Vars
#------------------------------------


#------------------------------------
# Dependencies
#------------------------------------
# Boost
set(BOOST_ALL_DYN_LINK ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)    
find_package(Boost 1.55 COMPONENTS  serialization thread date_time filesystem system REQUIRED )
link_directories (${Boost_LIBRARY_DIRS})
include_directories (${Boost_INCLUDE_DIRS})
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

# Threads
find_package(Threads)

if(WIN32)
    find_library(PSAPI_LIBRARIES psapi)
endif()

#------------------------------------
# Project Setup
#------------------------------------
#### Project Files

#include_directories(${CMAKE_CURRENT_LIST_DIR} )
include_directories("${CMAKE_CURRENT_LIST_DIR}/stagecoach" )

file(GLOB_RECURSE SOURCES "stagecoach/*.cpp")


#### Project Build Targets
add_executable(stagecoachd  stagecoachd.cpp ${SOURCES} )
add_executable(stagecoachclient  stagecoachclient.cpp ${SOURCES} )


#### Linking
target_link_libraries(stagecoachd ${Boost_LIBRARIES} ${PSAPI_LIBRARIES} pthread)
target_link_libraries(stagecoachclient ${Boost_LIBRARIES} ${PSAPI_LIBRARIES} pthread)


	

#------------------------------------
# Installation
#------------------------------------


