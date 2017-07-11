cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES
        ../crc64.c
        ../crc64.h
        ../dwr.c
        ../dwr.h
        ../dwr_types.h
        ../dwrando-cli.c
        ../map.c
        ../map.h
        ../mt19937-64.c
        ../mt64.h)

SET(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER $ENV{HOME}/.local/bin/i686-w64-mingw32.static-gcc)
set(CMAKE_CXX_COMPILER $ENV{HOME}/.local/bin/i686-w64-mingw32.static-g++)

add_executable(dwrandomizer.exe ${SOURCE_FILES})

