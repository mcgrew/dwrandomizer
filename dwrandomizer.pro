
# Build instructions:
#
# mkdir build
# cd build
# qmake ../dwrandomizer.pro
# make
#

CONFIG += "c++11"
INCLUDEPATH = common/
HEADERS += common/dwr.h common/crc64.h common/build.h common/mt64.h
HEADERS += common/dwr_types.h common/map.h common/chaos.h common/sprites.h
HEADERS += common/patch.h common/challenge.h common/polyfit.h

SOURCES += common/mt19937-64.c common/map.c common/dwr.c
SOURCES += common/crc64.c common/chaos.c common/sprites.c
SOURCES += common/patch.c common/challenge.c common/polyfit.c

ALPHA {
    DEFINES += DWR_ALPHA
}

RELEASE {
    DEFINES += DWR_RELEASE
}

CLI {
    SOURCES += cli/main.c
    QT = ""
} ELSE {
    SOURCES += ui/main.cpp ui/main-window.cpp ui/widgets.cpp
    HEADERS += ui/main-window.h ui/widgets.h
    QT += widgets
    RC_ICONS = resources/icons/dwrandomizer.ico
}


