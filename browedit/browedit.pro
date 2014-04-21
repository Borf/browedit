TEMPLATE = app
CONFIG += console

CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
CONFIG += threads
CONFIG -= warn_on

QMAKE_CXXFLAGS += -Wall -Wno-unused-variable

INCLUDEPATH += ../blib
INCLUDEPATH += ../blib/externals
INCLUDEPATH += ../blib/externals/box2d

SOURCES += main.cpp \
    BrowEdit.cpp \
    Camera.cpp \
    windows/FileOpenWindow.cpp \
    windows/ObjectWindow.cpp \
    windows/TextureWindow.cpp

HEADERS += \
    BrowEdit.h \
    Camera.h \
    version.h \
    windows/FileOpenWindow.h \
    windows/ObjectWindow.h \
    windows/TextureWindow.h



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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../brolib/release/ -lbrolib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../brolib/debug/ -lbrolib
else:unix: LIBS += -L$$OUT_PWD/../brolib/ -lbrolib

INCLUDEPATH += $$PWD/../brolib
DEPENDPATH += $$PWD/../brolib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../brolib/release/libbrolib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../brolib/debug/libbrolib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../brolib/release/brolib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../brolib/debug/brolib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../brolib/libbrolib.a


LIBS += -lpthread
LIBS += -lGLEW
LIBS += -lGL
LIBS += -lX11
LIBS += -lz
