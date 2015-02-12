cmake_minimum_required(VERSION 2.8)

find_package(Threads REQUIRED)

if(UNIX)
    add_definitions(-D_LINUX -D_GNU_SOURCE)
endif()
add_library(gsp STATIC gsp.cpp abstract-gsp.cpp gsp-hid.cpp)
target_link_libraries(gsp ${CMAKE_THREAD_LIBS_INIT} hidapi tthread)
