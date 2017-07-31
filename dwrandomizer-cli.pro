
# This is the qmake project file for the command line version. This is
# normally only built for Linux users.

INCLUDEPATH = common/
HEADERS += common/dwr.h common/crc64.h common/build.h common/mt64.h
HEADERS += common/dwr_types.h common/map.h common/chaos.h

SOURCES += cli/main.c
SOURCES += common/mt19937-64.c common/map.c common/dwr.c
SOURCES += common/crc64.c common/chaos.c

QT = ""
