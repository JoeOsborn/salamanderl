#include "drawinfo.h"

DrawInfo drawinfo_new() {
  return calloc(1, sizeof(struct _drawinfo));
}
DrawInfo drawinfo_init(DrawInfo inf, int z, TCOD_color_t fore, TCOD_color_t bg, char symbol) {
  inf->z = z;
  inf->fore = fore;
  inf->back = bg;
  inf->symbol = symbol;
  return inf;
}
void drawinfo_free(DrawInfo inf) {
  free(inf);
}

TCOD_color_t drawinfo_fore_color(DrawInfo inf) {
  return inf ? inf->fore : (TCOD_color_t){255, 255, 255};
}
TCOD_color_t drawinfo_back_color(DrawInfo inf) {
  return inf ? inf->back : (TCOD_color_t){0, 0, 0};
}
char drawinfo_symbol(DrawInfo inf) {
  return inf ? inf->symbol : ' ';
}
int drawinfo_z(DrawInfo inf) {
  return inf ? inf->z : 0;
}
