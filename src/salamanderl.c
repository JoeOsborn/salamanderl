/*
 * libtcod C samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */
#include <stdlib.h> /* for NULL */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "libtcod.h"

/* sample screen size */
#define SAMPLE_SCREEN_WIDTH 46
#define SAMPLE_SCREEN_HEIGHT 20
/* sample screen position */
#define SAMPLE_SCREEN_X 20
#define SAMPLE_SCREEN_Y 10

/* ***************************
 * the main function
 * ***************************/
int main( int argc, char *argv[] ) {
  char *font="libtcod/fonts/courier12x12_aa_tc.png";
  int nb_char_horiz=0,nb_char_vertic=0;
  int font_flags=TCOD_FONT_TYPE_GREYSCALE|TCOD_FONT_LAYOUT_TCOD;
	TCOD_console_set_custom_font(font,font_flags,nb_char_horiz,nb_char_vertic);
	TCOD_console_init_root(80,40,"salamandeRL",false);
	#define MAP_SZ 24
	static const char *map[MAP_SZ] = {
	  "################################################################################",
    "################################################################################",
	  "######............#########################################....................#",
    "######............#########################################....................#",
	  "######............#########################################....................#",
    "######............#########################################....................#",
	  "######............#########################################....................#",
    "######.........................................................................#",
	  "######............#########################################....................#",
    "######............#########################################....................#",
	  "######.........................................................................#",
    "######.........................................................................#",
	  "######............#########################################....................#",
    "######............#########################################....................#",
	  "######.........................................................................#",
    "######.........................................................................#",
	  "######.........................................................................#",
    "######............#########################################....................#",
	  "######............#########################################....................#",
    "######............#########################################....................#",
	  "######............#########################################....................#",
    "#############..###################################################..############",
	  "######.........................................................................#",
	  "################################################################################"
  };
  int i;
  int atX = 7, atY=13;
  float elapsed = 0;
	TCOD_key_t key = {TCODK_NONE,0};
	do {
		TCOD_console_set_foreground_color(NULL,TCOD_white);
    for(i = 0; i < MAP_SZ; i++) {
      TCOD_console_print_left(NULL, 0, i, TCOD_BKGND_NONE, map[i]);
    }
    TCOD_console_print_left(NULL, atX, atY, TCOD_BKGND_NONE, "@");
		TCOD_console_print_right(NULL,79,26,TCOD_BKGND_NONE,"last frame : %3d ms (%3d fps)", (int)(TCOD_sys_get_last_frame_length()*1000), TCOD_sys_get_fps());
		TCOD_console_print_right(NULL,79,27,TCOD_BKGND_NONE,"elapsed : %8dms %4.2fs", TCOD_sys_elapsed_milli(),TCOD_sys_elapsed_seconds());
		TCOD_console_print_left(NULL,0,27,TCOD_BKGND_NONE,"other stat stuff can go here");
		//map
		
		//divider
		TCOD_console_print_left(NULL,0,28,TCOD_BKGND_NONE,
		  "--------------------------------------------------------------------------------"
		);
	  //text display
    TCOD_console_print_left(NULL,2,29,TCOD_BKGND_NONE,"we'll probably put text down here.");
		
		/* update the game screen */
		TCOD_console_flush();

		/* did the user hit a key ? */
		key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
		if ( key.vk == TCODK_DOWN ) {
			/* down arrow */
			if(atY < 23) {
        atY++;
			}
		} else if ( key.vk == TCODK_UP ) {
			/* up arrow  */
			if(atY > 0) {
        atY--;
			}
    } else if ( key.vk == TCODK_LEFT ) {
      if(atX > 0) {
        atX--;
      }
    } else if ( key.vk == TCODK_RIGHT ) {
      if(atX < 79) {
        atX++;
      }
    }
	} while (!TCOD_console_is_window_closed());
	return 0;
}


