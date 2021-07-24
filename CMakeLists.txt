# This is the cmake build file for the project. This works on Linux, but
# not Windows, so I will be using qmake instead. Keeping this around for
# a bit, just in case.

cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compile the command line version
include_directories(common)

set(SOURCE_FILES
        common/crc64.c
        common/sprites.c
        common/dwr.c
        common/map.c
        common/mt19937-64.c
        common/chaos.c
        common/patch.c
        common/challenge.c
        common/polyfit.c)

# emscripten build command
#  cmake -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake ..
if (EMSCRIPTEN)
    add_executable(dwrandomizer ${SOURCE_FILES})
    target_link_libraries(dwrandomizer m)
    target_link_options(dwrandomizer PRIVATE
        "SHELL:
        -s EXPORTED_FUNCTIONS='_dwr_randomize,_sprite_name,_version'
        -s EXPORTED_RUNTIME_METHODS='ccall,cwrap'
        -s WASM_BIGINT")
else()
    add_executable(dwrandomizer cli/main.c ${SOURCE_FILES})
    target_link_libraries(dwrandomizer m)
endif()
