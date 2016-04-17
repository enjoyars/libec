#####################################################################
# Cross platform Enjoy Clicker SDK
# By GMiaow <miaow1988@qq.com>
#####################################################################
cmake_minimum_required(VERSION 2.8)

project(libec)
set(CMAKE_INSTALL_PREFIX ./${PROJECT_NAME})

#####################################################################
# Compiler option
# 1. Use static link library.
# 2. No warning about sprintf.
#####################################################################
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif()
    endforeach()
endif()
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

#####################################################################
# Project Source
####################################################################
add_subdirectory(src)

install(FILES README.md DESTINATION .)

#####################################################################
# Package
#####################################################################
set(CPACK_PACKAGE_VENDOR "EnjoyARS")
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 3)
set(CPACK_PACKAGE_VERSION_PATCH 5)
include(CPack)
