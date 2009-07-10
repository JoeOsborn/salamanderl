#ifndef _DRAWINFO_H
#define _DRAWINFO_H

#include <libtcod.h>

struct _drawinfo {
  int z;
  TCOD_color_t fore;
  TCOD_color_t back;
  char symbol;
};

typedef struct _drawinfo * DrawInfo;

DrawInfo drawinfo_new();
DrawInfo drawinfo_init(DrawInfo inf, int z, TCOD_color_t fore, TCOD_color_t bg, char symbol);
void drawinfo_free(DrawInfo inf);

TCOD_color_t drawinfo_fore_color(DrawInfo inf);
TCOD_color_t drawinfo_back_color(DrawInfo inf);
char drawinfo_symbol(DrawInfo inf);
int drawinfo_z(DrawInfo inf);

DrawInfo drawinfo_get_z_level(TCOD_list_t drawInfos, int senseZ, int posZ);

#endif