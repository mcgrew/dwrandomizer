cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_AUTOMOC ON)
SET(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER $ENV{HOME}/.local/bin/i686-w64-mingw32.static-gcc)
set(CMAKE_CXX_COMPILER $ENV{HOME}/.local/bin/i686-w64-mingw32.static-g++)

find_package(Qt5 REQUIRED COMPONENTS Gui Widgets)
include_directories(../common ../../mxe/usr/include/qt)

set(SOURCE_FILES
        ../common/crc64.c
        ../common/dwr.c
        ../common/map.c
        ../common/mt19937-64.c
        ../ui/main.cpp
        ../ui/main-window.cpp
        ../ui/widgets.cpp)

add_executable(dwrandomizer.exe ${SOURCE_FILES})

target_link_libraries(dwrandomizer.exe Qt5::Widgets Qt5::Gui)
