TEMPLATE = subdirs
CONFIG += c++11
QMAKE_CXXFLAGS += -Wno-unused-variable

SUBDIRS += blib/blib.pro \
    brolib \
    browedit
