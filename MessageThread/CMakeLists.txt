cmake_minimum_required(VERSION 3.5)
set(PROJ_NAME Message)
message(" --------- ${PROJ_NAME} begin cmake --------- ")
#include("${CMAKE_SOURCE_DIR}/Gst.cmake")

## ----------------------------------------------------------- ##
#  Message
## ----------------------------------------------------------- ##


set(INC_LIST "${GST_INC}"
            "${CMAKE_SOURCE_DIR}/"
             )

set(SRC_LIST 
            "${CMAKE_SOURCE_DIR}/Message.cpp"
            "${CMAKE_SOURCE_DIR}/MessageHandler.cpp"
            "${CMAKE_SOURCE_DIR}/MessageLooper.cpp"
            "${CMAKE_SOURCE_DIR}/MessageQueue.cpp"
            "${CMAKE_SOURCE_DIR}/MessageThread.cpp"
            "${CMAKE_SOURCE_DIR}/TestThread.cpp"
            "${CMAKE_SOURCE_DIR}/MyHandler.cpp"
            "${CMAKE_SOURCE_DIR}/Thread.cpp"
            "${CMAKE_SOURCE_DIR}/main.cpp"
             )


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(LOCAL_CXXFLAGS -g -fPIC )
else()
set(LOCAL_CXXFLAGS -O2 -fPIC)
endif()

##----------cmake set-----------##
message("${PROJ_NAME} begin cmake")

project(${PROJ_NAME})

include_directories(${INC_LIST})
add_compile_options(${LOCAL_CXXFLAGS})

add_executable(${PROJ_NAME} ${SRC_LIST})

#temp to add all src files in the dir
#aux_source_directory(${CMAKE_SOURCE_DIR}/ DIR_SRCS)

#add_executable(${PROJ_NAME} ${DIR_SRCS})

target_link_libraries(
    ${PROJ_NAME}
    pthread
)

set_target_properties (
    ${PROJ_NAME}
    PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)