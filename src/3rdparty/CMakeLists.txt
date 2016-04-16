cmake_minimum_required(VERSION 2.8)

# hidapi-0.7.0
# http://www.signal11.us/oss/hidapi/
include_directories(hidapi)
if(WIN32)
    add_library(hidapi STATIC hidapi/windows/hid.c)
    target_link_libraries(hidapi setupapi.lib)
elseif(APPLE)
    find_library(IOKit_PATH NAMES IOKit)
    find_library(CoreFoundation_PATH NAMES CoreFoundation)
    add_library(hidapi STATIC hidapi/mac/hid.c)
    target_link_libraries(hidapi ${IOKit_PATH} ${CoreFoundation_PATH})
elseif(UNIX)
    add_library(hidapi STATIC hidapi/linux/hid.c)
    target_link_libraries(hidapi udev)
endif()

# TinyThread++-1.1
# http://tinythreadpp.bitsnbites.eu/
include_directories(tthread)
add_library(tthread STATIC tthread/tinythread.cpp)
