# This is the cmake build file for the project. This works on Linux, but
# not Windows, so I will be using qmake instead. Keeping this around for
# a bit, just in case.

cmake_minimum_required(VERSION 3.7)
project(dwrandomizer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compile the command line version
include_directories(common)

set(SOURCE_FILES
        common/crc64.c
        common/sprites.c
        common/dwr.c
        common/map.c
        common/dungeon_maps.c
        common/stair_shuffle.c
        common/chests.c
        common/expansion.c
        common/mt19937-64.c
        common/chaos.c
        common/patch.c
        common/challenge.c
        common/polyfit.c
        common/base32.c)

if (ALPHA)
    add_definitions(-DDWR_ALPHA)
elseif(RELEASE)
    add_definitions(-DDWR_RELEASE)
endif()

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
    file(REMOVE_RECURSE                                        ${CMAKE_BINARY_DIR}/release/)
    file(MAKE_DIRECTORY                                        ${CMAKE_BINARY_DIR}/release/)
    file(CREATE_LINK ${CMAKE_SOURCE_DIR}/webui/index.html      ${CMAKE_BINARY_DIR}/release/index.html)
    file(CREATE_LINK ${CMAKE_SOURCE_DIR}/webui/index.js        ${CMAKE_BINARY_DIR}/release/index.js)
    file(CREATE_LINK ${CMAKE_SOURCE_DIR}/webui/index.css       ${CMAKE_BINARY_DIR}/release/index.css)
    file(CREATE_LINK ${CMAKE_SOURCE_DIR}/webui/base32.js       ${CMAKE_BINARY_DIR}/release/base32.js)
    file(CREATE_LINK ${CMAKE_SOURCE_DIR}/webui/interface.js    ${CMAKE_BINARY_DIR}/release/interface.js)
    file(CREATE_LINK ${CMAKE_SOURCE_DIR}/webui/sprites         ${CMAKE_BINARY_DIR}/release/sprites SYMBOLIC)
    add_custom_command(TARGET dwrandomizer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dwrandomizer.js ${CMAKE_BINARY_DIR}/release/)
    add_custom_command(TARGET dwrandomizer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/dwrandomizer.wasm ${CMAKE_BINARY_DIR}/release/)
else()
    add_executable(dwrandomizer cli/main.c ${SOURCE_FILES})
    target_link_libraries(dwrandomizer m)
endif()
