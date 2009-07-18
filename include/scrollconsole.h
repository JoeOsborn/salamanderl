#ifndef _SCROLL_CONSOLE_H
#define _SCROLL_CONSOLE_H

#include <libtcod.h>

struct _scroll_console {
  TCOD_console_t cons;
  int w, h;
  TCOD_list_t lines;
};
typedef struct _scroll_console *ScrollConsole;

ScrollConsole scrollconsole_new();
ScrollConsole scrollconsole_init(ScrollConsole sc, int w, int h);
void scrollconsole_free(ScrollConsole sc);

void scrollconsole_push(ScrollConsole sc, char *msg);
void scrollconsole_blit(ScrollConsole sc, TCOD_console_t dest, int destX, int destY, char destOpacity);

#endif