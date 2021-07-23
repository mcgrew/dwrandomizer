# This is the cmake build file for the project. This works on Linux, but
# not Windows, so I will be using qmake instead. Keeping this around for
# a bit, just in case.

cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
find_package(Qt5 COMPONENTS Gui Widgets)

# Compile the command line version
include_directories(common)

set(CLI_SOURCE_FILES
        common/crc64.c
        common/sprites.c
        common/dwr.c
        common/map.c
        common/mt19937-64.c
        common/chaos.c
        common/patch.c
        common/challenge.c
        common/polyfit.c
        cli/main.c)

add_executable(dwrandomizer-cli ${CLI_SOURCE_FILES})
target_link_libraries(dwrandomizer-cli m)

if (EMSCRIPTEN)
    target_link_options(dwrandomizer-cli PRIVATE "SHELL:-s EXPORTED_FUNCTIONS='[\"_dwr_randomize\"]' -s
    EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\"]'")
endif()
if(Qt5Widgets_FOUND)
    set (CMAKE_AUTOMOC ON)
    include_directories(common /usr/include/qt)
    set (UI_SOURCE_FILES
            common/crc64.c
            common/sprites.c
            common/dwr.c
            common/map.c
            common/mt19937-64.c
            common/chaos.c
            common/patch.c
            common/challenge.c
            common/polyfit.c
            ui/main-window.cpp
            ui/widgets.cpp
            ui/main.cpp)


    add_executable(dwrandomizer ${UI_SOURCE_FILES})

    target_link_libraries(dwrandomizer Qt5::Widgets Qt5::Gui)
else ()
  message("Could not find Qt modules, skipping GUI build.")
endif()
