SRCDIR=./src
TCINCDIR=tilesense/libtcod/include
CC=gcc
CPP=g++
FLAGS=-ggdb -std=c99
TEMP=/tmp
.SUFFIXES: .o .h .c .hpp .cpp

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

CFLAGS=$(FLAGS) -I$(TCINCDIR) -I$(TCINCDIR)/gui -Wall $(TS_FLAGS) $(SDL_FLAGS) $(ZLIB_FLAGS) $(PNG_FLAGS) -fno-strict-aliasing

$(TEMP)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

C_OBJS=$(TEMP)/salamanderl.o

salamanderl : $(C_OBJS)
	$(CC) $(C_OBJS) $(CFLAGS) -o $@ -L./tilesense/libtcod -ltcod -L./tilesense -ltilesense $(ZLIB_LIBS) $(PNG_LIBS) $(SDL_LIBS) 

salamanderl.app : salamanderl
	mkdir -p salamanderl.app/Contents/MacOS
	cp mac_Info.plist salamanderl.app/Contents/Info.plist
	cp salamanderl salamanderl.app/Contents/MacOS/

clean :
	\rm -f $(C_OBJS) salamanderl
	\rm -rf salamanderl.app
