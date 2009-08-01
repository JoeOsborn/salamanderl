#include "scrollconsole.h"
#include <stdlib.h>
#include <string.h>
#include <tilesense.h>

ScrollConsole scrollconsole_new() {
  return calloc(1, sizeof(struct _scroll_console));
}
ScrollConsole scrollconsole_init(ScrollConsole sc, int w, int h) {
  sc->w = w;
  sc->h = h;
  sc->cons = TCOD_console_new(w, h);
  sc->lines = TCOD_list_new();
  return sc;
}
void scrollconsole_free(ScrollConsole sc) {
  TCOD_console_delete(sc->cons);
  TCOD_list_clear_and_delete(sc->lines);
  free(sc);
}

void scrollconsole_push(ScrollConsole sc, char *msg) {
  //for each line in msg:
    //if the buffer is full:
      //should shuffle all existing text up by one line
      //then print line at the bottom
    //otherwise: print line as far down as there is text.
  TCOD_list_push(sc->lines, strdup(msg));
  //now, figure out mappings from lines to y values.
  //now, count backwards and count the number of real lines each line takes up.  When we get past our height, that's the number of lines we want.
  int lineCount = 0;
  int elements = 0;
  int lastLineCount = 0;
  for(int i = TCOD_list_size(sc->lines)-1; i >= 0; i--) {
    char *line = TCOD_list_get(sc->lines, i);
    lastLineCount = TCOD_console_height_left_rect(sc->cons, 0, 0, sc->w, sc->h, line);
    lineCount += lastLineCount;
    elements++;
    //we have more lines than we need.  first of all, we need to counteract the element addition.
    if(lineCount > sc->h) {
      elements--;
      //now, pop off the last dealie and insert blank lines to fill the empty space
      int fullLines = (lineCount - lastLineCount);
      int underFlow = sc->h - fullLines - 1;
      for(int j = 0; j < underFlow; j++) {
        lineCount--;
        TCOD_list_insert_before(sc->lines, strdup(" "), i+1);
        //we want this blank line in the output
        elements++;
      }
    }
    if(lineCount == sc->h) {
      //this is the last line, break.s
      break;
    }
  }
  //now, chop off the lines we won't be printing any of
  while(TCOD_list_size(sc->lines) > elements) {
    char *l = TCOD_list_get(sc->lines, 0);
    TCOD_list_remove(sc->lines, l);
    free(l);
  }
  //now, insert blank lines at the beginning if we're in the unenviable position of scrolling out an old line
  TCOD_console_clear(sc->cons);
  int currentY = (lineCount > sc->h) ? sc->h : lineCount;
  for(int i = TCOD_list_size(sc->lines)-1; i >= 0; i--) {
    char *line = TCOD_list_get(sc->lines, i);
    currentY -= TCOD_console_height_left_rect(sc->cons, 0, 0, sc->w, sc->h, line);
    TCOD_console_print_left_rect(sc->cons, 0, currentY, sc->w, sc->h, line);
    if(currentY < 0) {
      break;
    }
  }
}
void scrollconsole_blit(ScrollConsole sc, TCOD_console_t dest, int destX, int destY, char destOpacity) {
  TCOD_console_blit(sc->cons, 0, 0, sc->w, sc->h, dest, destX, destY, destOpacity, destOpacity);
}
