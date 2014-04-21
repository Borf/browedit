TEMPLATE = lib
CONFIG += staticlib
CONFIG += object_with_source
CONFIG += object_parallel_to_source
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG -= warn_on

QMAKE_CXXFLAGS += -Wall -Wno-unused-variable

INCLUDEPATH += ../blib
INCLUDEPATH += ../blib/externals
INCLUDEPATH += ../blib/externals/box2d

SOURCES += \
    BroLib/Gnd.cpp \
    BroLib/GrfFileSystemHandler.cpp \
    BroLib/Map.cpp \
    BroLib/MapRenderer.cpp \
    BroLib/Rsm.cpp \
    BroLib/Rsw.cpp \
    BroLib/grflib/grf.c \
    BroLib/grflib/grfcrypt.c \
    BroLib/grflib/grfsupport.c \
    BroLib/grflib/rgz.c

HEADERS += \
    BroLib/Gnd.h \
    BroLib/GrfFileSystemHandler.h \
    BroLib/Map.h \
    BroLib/MapRenderer.h \
    BroLib/Renderer.h \
    BroLib/Rsm.h \
    BroLib/Rsw.h \
    BroLib/grflib/grf.h \
    BroLib/grflib/grfcrypt.h \
    BroLib/grflib/grfsupport.h \
    BroLib/grflib/grftypes.h \
    BroLib/grflib/rgz.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../blib/release/ -lblib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../blib/debug/ -lblib
else:unix: LIBS += -L$$OUT_PWD/../blib/ -lblib

INCLUDEPATH += $$PWD/../blib
DEPENDPATH += $$PWD/../blib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../blib/release/libblib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../blib/debug/libblib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../blib/release/blib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../blib/debug/blib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../blib/libblib.a
