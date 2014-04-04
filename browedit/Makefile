#!make
LIBS = ../blib/blib_linux64.a ../brolib/brolib_linux64.a -lX11 -lz -lpthread -lGL -lGLEW
INCLUDES = -I. -I../brolib -I../blib -I../blib/externals
LD = g++
CXX = g++
CC = gcc
DEFINES = 
CFLAGS = -Wall -pipe
CPPFLAGS = -Wall -pipe -std=c++0x
LDFLAGS = 
# where are source files located?
SOURCE_DIRS=. windows
# Host platform
UNAME=$(shell uname -s | sed -e 's/_.*$$//')

#####
## PLATFORM DETECTION CODE
#####
ifeq ($(PLATFORM),)
UNAME_CPU=$(shell uname -m)

## Cygwin
ifeq ($(UNAME),CYGWIN)
# can't do linux build anyway on cygwin
PLATFORM=win32
endif

## Linux
ifeq ($(UNAME),Linux)
# detect 64bit
ifeq ($(UNAME_CPU),x86_64)
PLATFORM=linux64
else
PLATFORM=linux32
endif
endif

endif

#####
## END OF PLATFORM DETECTION CODE
#####

#####
## PER-PLATFORM SETTINGS
#####

# Linux 32bit
ifeq ($(PLATFORM),linux32)
CFLAGS += -m32
CPPFLAGS += -m32
CC=gcc
CXX=g++
BINARY_EXT=
endif

# Linux 64bit
ifeq ($(PLATFORM),linux64)
CFLAGS += -m64
CPPFLAGS += -m64
CC=gcc
CXX=g++
BINARY_EXT=
endif

# Windows 32bit
ifeq ($(PLATFORM),win32)
# Mh, we don't use GUI only mode, but console instead?
CFLAGS += -mconsole
ifeq ($(UNAME),CYGWIN)
# On cygwin, use provided gcc, but tell to not use cygwin's dlls
CC=gcc
CXX=g++
CFLAGS += -mno-cygwin
else
CC=mingw32-gcc
CXX=mingw32-g++
endif
WINDRES=mingw32-windres
BINARY_EXT=.exe
INCLUDES += -Ilibs/include
LIBS = -L. -lzlib1 -lSDL -lbgd -lopengl32 -lglu32 -lws2_32 -lcomdlg32
# ws2_32.lib sdl.lib sdlmain.lib zlib.lib bgd.lib opengl32.lib glu32.lib 
endif

#####
## END OF PER-PLATFORM SETTINGS
#####

## Debug build?
#ifeq ($(DEBUG),)
DEBUG=yes
#endif

ifeq ($(DEBUG),yes)
CFLAGS += -g -ggdb -fstack-protector-all -D_DEBUG -rdynamic
CPPFLAGS += -g -ggdb -fstack-protector-all -D_DEBUG -rdynamic
else
# We suppose everyone have SSE. If this cause problems, switch mfpmath back to 387
CFLAGS += -O3 -fomit-frame-pointer -momit-leaf-frame-pointer -mfpmath=sse
CPPFLAGS += -O3 -fomit-frame-pointer -momit-leaf-frame-pointer -mfpmath=sse
endif

TARGET=browedit_$(PLATFORM)

# Define path where make will find source files :)
VPATH=$(subst  ,:,$(SOURCE_DIRS))

# list all source files
SOURCE_ALL=$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c $(dir)/*.cpp))

# list all object files
OBJECTS_ALL=$(patsubst %,obj/%_$(PLATFORM).o,$(basename $(SOURCE_ALL)))

# list all .dep files
DEP_ALL=$(patsubst %,obj/%_$(PLATFORM).dep,$(basename $(filter %.c %.cpp,$(SOURCE_ALL))))

ifeq ($(PLATFORM),win32)
# Fix: Win32 build needs this one
OBJECTS_SRC += obj/src_Script1_rc_$(PLATFORM).o
endif

all: $(TARGET)

clean:
	$(RM) `find obj` $(TARGET)

# Depencies

.PHONY: dep

dep: obj/depencies_$(PLATFORM).mak

obj/depencies_$(PLATFORM).mak: $(DEP_ALL)
	@echo -e "    [DEP]	$@                          "
	@mkdir -p obj/brolib
# build depencies for objects
	@cat $^ | sed -r -e 's#^([a-zA-Z0-9]+)\.o#obj/\1_$(PLATFORM).o#' >$@
# build depencies for depencies
	@cat $^ | sed -r -e 's#^([a-zA-Z0-9]+)\.o#obj/\1_$(PLATFORM).dep#' >>$@

-include obj/depencies_$(PLATFORM).mak

# type-specific targets (compile, target = %.o)

obj/%_$(PLATFORM).o: %.c
	@echo -e "    [CC]	$<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

obj/%_rc_$(PLATFORM).o: %.rc
	@echo -e "    [RC]	$<"
	@$(WINDRES) -I src -i $< -o $@

obj/%_$(PLATFORM).o: %.cpp
	@echo -e "    [CC]	$<"
	@$(CXX) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<

# depencies

obj/%_$(PLATFORM).dep: %.c
	@echo -en "    [DEP]	$<                                   \r"
	@$(CC) $(CFLAGS) $(INCLUDES) -MM -MF $@ $<

obj/%_$(PLATFORM).dep: %.cpp
	@echo -en "    [DEP]	$<                                    \r"
	@$(CXX) $(CPPFLAGS) $(INCLUDES) -MM -MF $@ $<

# Main target

$(TARGET): $(OBJECTS_ALL)
	@echo -e "    [LD]	$@"
	@$(LD) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
# $(LIBS)


