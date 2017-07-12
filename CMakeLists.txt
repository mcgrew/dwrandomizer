cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES
        crc64.c
        crc64.h
        dwr.c
        dwr.h
        dwr_types.h
        dwrando-cli.c
        map.c
        map.h
        mt19937-64.c
        mt64.h)

#Linux build
add_executable(dwrandomizer ${SOURCE_FILES})
target_link_libraries(dwrandomizer m)

if (DEFINED WIN32)
    add_subdirectory(win32)
endif()

