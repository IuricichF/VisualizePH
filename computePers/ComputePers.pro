TEMPLATE = app

CONFIG -= app_bundle
CONFIG -= qt


# Directories
DESTDIR = dist/
OBJECTS_DIR = build/

#Standard configurations
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

mac : {
QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_LFLAGS += -lstdc++
}

INCLUDEPATH += source/include/


SOURCES += source/main.cpp

HEADERS += \
    source/mesh.h


