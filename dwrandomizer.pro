
# This is the qmake project file for the gui. This should apply to all platforms
#
# Build instructions:
#
# mkdir build
# cd build
# qmake ../dwrandomizer.pro && make
#

INCLUDEPATH = common/
HEADERS += ../ui/main-window.h ../ui/widgets.h ../common/dwr.h ../common/crc64.h
HEADERS += ../common/dwr_types.h ../common/map.h ../common/build.h ../common/mt64.h

SOURCES += ../common/mt19937-64.c ../common/map.c ../common/dwr.c ../common/crc64.c
SOURCES += ../ui/main.cpp ../ui/main-window.cpp ../ui/widgets.cpp

QT += widgets
