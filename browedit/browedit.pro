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
windows
{
    INCLUDEPATH += ../blib/externals/glew/include
}

DEFINES -= UNICODE



LIBS += -lopengl32
LIBS += -lglu32
LIBS += -luser32
LIBS += -lgdi32


SOURCES += main.cpp \
    BrowEdit.cpp \
    Camera.cpp \
    windows/FileOpenWindow.cpp \
    windows/ObjectWindow.cpp \
    windows/TextureWindow.cpp \
    actions/GroupAction.cpp \
    actions/ObjectEditAction.cpp \
    actions/SelectObjectAction.cpp \
    actions/TextureEditAction.cpp \
    windows/MessageWindow.cpp \
    windows/ModelPropertiesWindow.cpp \
    RotateTool.cpp \
    ScaleTool.cpp \
    TranslatorTool.cpp

HEADERS += \
    BrowEdit.h \
    Camera.h \
    version.h \
    windows/FileOpenWindow.h \
    windows/ObjectWindow.h \
    windows/TextureWindow.h \
    actions/GroupAction.h \
    actions/ObjectEditAction.h \
    actions/SelectObjectAction.h \
    actions/TextureEditAction.h \
    windows/MessageWindow.h \
    windows/ModelPropertiesWindow.h \
    RotateTool.h \
    ScaleTool.h \
    TranslatorTool.h



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


#windows
#{
#    LIBS += -lopengl32
#}
#
#unix
#{
#    LIBS += -lpthread
#    LIBS += -lGLEW
#    LIBS += -lGL
#    LIBS += -lX11
#    LIBS += -lz
#}
OTHER_FILES += \
    ../assets/shaders/compose.frag \
    ../assets/shaders/gnd.frag \
    ../assets/shaders/highlight.frag \
    ../assets/shaders/rsw.frag \
    ../assets/shaders/compose.vert \
    ../assets/shaders/gnd.vert \
    ../assets/shaders/highlight.vert \
    ../assets/shaders/rsw.vert
