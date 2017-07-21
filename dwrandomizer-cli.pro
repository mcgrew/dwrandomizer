
# This is the qmake project file for the command line version. This is
# normally only built for Linux users.

INCLUDEPATH = common/
HEADERS += ../common/dwr.h ../common/crc64.h ../common/dwr_types.h
HEADERS += ../common/map.h ../common/build.h ../common/mt64.h

SOURCES += ../common/mt19937-64.c ../common/map.c ../common/dwr.c
SOURCES += ../common/crc64.c cli/main.c

QT = ""
