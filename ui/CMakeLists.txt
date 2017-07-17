cmake_minimum_required(VERSION 3.7)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set (CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Gui Widgets)
include_directories(.. /usr/include/qt)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set (SOURCE_FILES ui_main.cpp widgets.cpp)


add_executable(ui_test ${SOURCE_FILES})

target_link_libraries(ui_test
        Qt5::Widgets
        Qt5::Gui
)

