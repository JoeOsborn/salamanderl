SRCDIR=./src
INCDIR=./include
TCINCDIR=tilesense/libtcod/include
CC=gcc
FLAGS=-g
CFLAGS=$(FLAGS) -std=gnu9x -I$(INCDIR) -I$(TCINCDIR) -I$(TCINCDIR)/gui -Wall $(TS_FLAGS) $(SDL_FLAGS) $(ZLIB_FLAGS) $(PNG_FLAGS) -fno-strict-aliasing
.SUFFIXES: .o .h .c
TMPDIR=/tmp/salamanderl

C_OBJS=\
	$(TMPDIR)/action/action.o \
	$(TMPDIR)/action/bindings.o \
	$(TMPDIR)/action/check.o \
	$(TMPDIR)/action/comparison.o \
	$(TMPDIR)/action/condition.o \
	$(TMPDIR)/action/effect_feed.o \
	$(TMPDIR)/action/effect_grab.o \
	$(TMPDIR)/action/effect_grantrevoke.o \
	$(TMPDIR)/action/effect_let_go.o \
	$(TMPDIR)/action/effect_pick_up.o \
	$(TMPDIR)/action/effect_put_down.o \
	$(TMPDIR)/action/effect_set.o \
	$(TMPDIR)/action/effect_message.o \
	$(TMPDIR)/loader/configlistener.o \
	$(TMPDIR)/loader/maplistener.o \
	$(TMPDIR)/loader/model_init_structrecord.o \
	$(TMPDIR)/loader/model_init_parser.o \
	$(TMPDIR)/loader/objectlistener.o \
	$(TMPDIR)/loader/prop.o \
	$(TMPDIR)/loader/savelistener.o \
	$(TMPDIR)/loader/statuslistener.o \
	$(TMPDIR)/loader/structrecord.o \
	$(TMPDIR)/loader.o \
	$(TMPDIR)/drawinfo.o \
	$(TMPDIR)/moveinfo.o \
	$(TMPDIR)/objectinfo.o \
	$(TMPDIR)/salamanderl.o \
	$(TMPDIR)/scrollconsole.o \
	$(TMPDIR)/sobject.o \
	$(TMPDIR)/status.o \
	$(TMPDIR)/tileinfo.o

HEADERS=\
	$(INCDIR)/action/action.h \
	$(INCDIR)/action/bindings.h \
	$(INCDIR)/action/check.h \
	$(INCDIR)/action/comparison.h \
	$(INCDIR)/action/condition.h \
	$(INCDIR)/action/effect_feed.h \
	$(INCDIR)/action/effect_grab.h \
	$(INCDIR)/action/effect_grantrevoke.h \
	$(INCDIR)/action/effect_let_go.h \
	$(INCDIR)/action/effect_pick_up.h \
	$(INCDIR)/action/effect_put_down.h \
	$(INCDIR)/action/effect_message.h \
	$(INCDIR)/action/effect_set.h \
	$(INCDIR)/loader/configlistener.h \
	$(INCDIR)/loader/maplistener.h \
	$(INCDIR)/loader/model_init_structrecord.h \
	$(INCDIR)/loader/model_init_parser.h \
	$(INCDIR)/loader/objectlistener.h \
	$(INCDIR)/loader/prop.h \
	$(INCDIR)/loader/savelistener.h \
	$(INCDIR)/loader/statuslistener.h \
	$(INCDIR)/loader/structrecord.h \
	$(INCDIR)/loader.h \
	$(INCDIR)/drawinfo.h \
	$(INCDIR)/moveinfo.h \
	$(INCDIR)/objectinfo.h \
	$(INCDIR)/scrollconsole.h \
	$(INCDIR)/sobject.h \
	$(INCDIR)/status.h \
	$(INCDIR)/tileinfo.h \
	$(INCDIR)/uthash.h \
	$(INCDIR)/utlist.h

$(TMPDIR) :
	mkdir -p $@
$(TMPDIR)/action : $(TMPDIR)
	mkdir -p $@
$(TMPDIR)/loader : $(TMPDIR)
	mkdir -p $@

$(TMPDIR)/action/%.o : $(SRCDIR)/action/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(TMPDIR)/loader/%.o : $(SRCDIR)/loader/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(TMPDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

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

salamanderl : $(TMPDIR)/action $(TMPDIR)/loader $(C_OBJS) 
	$(CC) $(C_OBJS) $(CFLAGS) -o $@ -L./tilesense/libtcod -ltcod-debug -L./tilesense -ltilesense $(ZLIB_LIBS) $(PNG_LIBS) $(SDL_LIBS) 

salamanderl.app : salamanderl
	mkdir -p $@/Contents/MacOS
	cp mac_Info.plist $@/Contents/Info.plist
	cp salamanderl $@/Contents/MacOS/

clean :
	rm -rf $(TMPDIR) salamanderl
	rm -rf salamanderl.app
