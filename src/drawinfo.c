#include "drawinfo.h"

DrawInfo drawinfo_new() {
  return calloc(1, sizeof(struct _drawinfo));
}
DrawInfo drawinfo_init(DrawInfo inf, TCOD_color_t fore, TCOD_color_t bg, char symbol) {
  inf->fore = fore;
  inf->back = bg;
  inf->symbol = symbol;
  return inf;
}
void drawinfo_free(DrawInfo inf) {
  free(inf);
}

TCOD_color_t drawinfo_fore_color(DrawInfo inf) {
  return inf->fore;
}
TCOD_color_t drawinfo_back_color(DrawInfo inf) {
  return inf->back;
}
char drawinfo_symbol(DrawInfo inf) {
  return inf->symbol;
}
