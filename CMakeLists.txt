# This is the cmake build file for the project. This works on Linux, but
# not Windows, so I will be using qmake instead. Keeping this around for
# a bit, just in case.

cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set (CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Gui Widgets)
include_directories(common /usr/include/qt)

set(CLI_SOURCE_FILES
        common/crc64.c
        common/dwr.c
        common/map.c
        common/mt19937-64.c
        common/chaos.c
        cli/main.c)

add_executable(dwrandomizer-cli ${CLI_SOURCE_FILES})
target_link_libraries(dwrandomizer-cli m)


set (UI_SOURCE_FILES
        common/crc64.c
        common/dwr.c
        common/map.c
        common/mt19937-64.c
        common/chaos.c
        ui/main-window.cpp
        ui/widgets.cpp
        ui/main.cpp)


add_executable(dwrandomizer ${UI_SOURCE_FILES})

target_link_libraries(dwrandomizer Qt5::Widgets Qt5::Gui)
