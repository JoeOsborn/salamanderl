#ifndef _DRAWINFO_H
#define _DRAWINFO_H

#include <libtcod.h>

struct _drawinfo {
  TCOD_color_t fore;
  TCOD_color_t back;
  char symbol;
};

typedef struct _drawinfo * DrawInfo;

DrawInfo drawinfo_new();
DrawInfo drawinfo_init(DrawInfo inf, TCOD_color_t fore, TCOD_color_t bg, char symbol);
void drawinfo_free(DrawInfo inf);

TCOD_color_t drawinfo_fore_color(DrawInfo inf);
TCOD_color_t drawinfo_back_color(DrawInfo inf);
char drawinfo_symbol(DrawInfo inf);

#endif