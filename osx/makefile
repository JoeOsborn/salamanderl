SRCDIR=./src
INCDIR=./include
TCINCDIR=tilesense/libtcod/include
CC=gcc
FLAGS=-ggdb -std=gnu9x
.SUFFIXES: .o .h .c

C_OBJS=\
  $(SRCDIR)/loader/loaderlistener.o \
  $(SRCDIR)/loader/prop.o \
  $(SRCDIR)/loader/structrecord.o \
  $(SRCDIR)/loader.o \
  $(SRCDIR)/drawinfo.o \
  $(SRCDIR)/moveinfo.o \
  $(SRCDIR)/objectinfo.o \
  $(SRCDIR)/tileinfo.o \
  $(SRCDIR)/salamanderl.o 

HEADERS=\
  $(INCDIR)/loader/loaderlistener.h \
  $(INCDIR)/loader/prop.h \
  $(INCDIR)/loader/structrecord.h \
  $(INCDIR)/loader.h \
  $(INCDIR)/drawinfo.h \
  $(INCDIR)/moveinfo.h \
  $(INCDIR)/objectinfo.h \
  $(INCDIR)/tileinfo.h \
  $(INCDIR)/uthash.h \
  $(INCDIR)/utlist.h

#dependencies
# SDL
SDL_DIR=tilesense/libtcod/dependencies/SDL-1.2.12
SDL_FLAGS=-D_GNU_SOURCE=1 -D_REENTRANT -I $(SDL_DIR)/include
SDL_LIBS=$(SDL_DIR)/lib/osx/libSDL.a $(SDL_DIR)/lib/osx/libSDLmain.a -framework Carbon -framework Quartz -framework OpenGL -framework AppKit -framework IOKit -framework AudioUnit -framework Quicktime -framework Cocoa

# ZLIB
ZLIB_DIR=tilesense/libtcod/dependencies/zlib-1.2.3
ZLIB_FLAGS=-I $(ZLIB_DIR)/include
ZLIB_LIBS=$(ZLIB_DIR)/lib/osx/libz.a

# PNG
PNG_DIR=tilesense/libtcod/dependencies/libpng-1.2.31
PNG_FLAGS=-I $(PNG_DIR)/include
PNG_LIBS=$(PNG_DIR)/lib/osx/libpng.a

# tilesense
TS_DIR=tilesense
TS_FLAGS=-I $(TS_DIR)/include
TS_LIBS=$(TS_DIR)/libtilesense.a

CFLAGS=$(FLAGS) -I$(INCDIR) -I$(TCINCDIR) -I$(TCINCDIR)/gui -Wall $(TS_FLAGS) $(SDL_FLAGS) $(ZLIB_FLAGS) $(PNG_FLAGS) -fno-strict-aliasing

salamanderl : $(C_OBJS) 
	$(CC) $(C_OBJS) $(CFLAGS) -o $@ -L./tilesense/libtcod -ltcod-debug -L./tilesense -ltilesense $(ZLIB_LIBS) $(PNG_LIBS) $(SDL_LIBS) 

salamanderl.app : salamanderl
	mkdir -p $@/Contents/MacOS
	cp mac_Info.plist $@/Contents/Info.plist
	cp salamanderl $@/Contents/MacOS/

clean :
	rm -f $(C_OBJS) salamanderl
	rm -rf salamanderl.app
