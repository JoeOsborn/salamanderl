#include <stdlib.h> /* for NULL */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <libtcod.h>

#include <tilesense.h>

Map initRoom() {
  Map m = map_new();
  unsigned short tileMap[] = {
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 2, 2, 2, 2, 2, 2,    
    
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 2, 2, 2, 2, 2,    
    
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 1, 2, 2, 1, 1, 2,
    2, 1, 1, 2, 2, 1, 1, 2,
    2, 1, 1, 1, 1, 1, 1, 2,
    2, 1, 1, 1, 1, 1, 1, 2,
    2, 1, 1, 2, 1, 1, 2, 2   
    };
  m = map_init(m, 
    "cage", 
    (mapVec){8, 8, 4}, 
    tileMap,
    3
  ); 
  Tile floorTile = tile_init(
    tile_new(), 
    0
  );
  Tile wallTile = tile_init(
    tile_new(), 
    3
  );
  map_add_tile(m, floorTile);
  map_add_tile(m, wallTile);
  return m;
}

Object initPlayer(Map room) {
  Object playerObj = object_init(object_new(), 
    "@", 
    (mapVec){3, 1, 0}, 
    (mapVec){1, 1, 0},
    room
  );
  Sensor leftEye = sensor_init(sensor_new(), "left_eye",
    frustum_init(frustum_new(),
      mapvec_zero,
      (mapVec){1, -1, 0},
      1, 2,
      0, 10
    )
  );
  Sensor rightEye = sensor_init(sensor_new(), "right_eye",
    frustum_init(frustum_new(),
      mapvec_zero,
      (mapVec){1, 1, 0},
      1, 2,
      0, 10
    )
  );
  Sensor basicSense = sensor_init(sensor_new(), "basic_sense",
    sphere_init(sphere_new(),
      mapvec_zero,
      2
    )
  );
  object_add_sensor(playerObj, leftEye);
  object_add_sensor(playerObj, rightEye);
  object_add_sensor(playerObj, basicSense);
  
  map_add_object(room, playerObj);
  
  return playerObj;
}

void drawtiles(Map m, unsigned char *buf, Sensor s, mapVec pos, mapVec size) {
  int index=0;
  unsigned char tileIndex;
  unsigned char flags;
  int drawX, drawY;
  Volume vol = sensor_volume(s);
  mapVec borig, bsz;
  volume_swept_bounds(vol, &borig, &bsz);
  mapVec msz = map_size(m);
  float ystart = CLIP(pos.y, 0, msz.y);
  float xstart = CLIP(pos.x, 0, msz.x);
  float yend = CLIP(pos.y+size.y, 0, msz.y);
  float xend = CLIP(pos.x+size.x, 0, msz.x);
  int z = CLIP(pos.z, 0, msz.z); //note: different from test.c version
  for(int y = ystart; y < yend; y++) {
    for(int x = xstart; x < xend; x++) {
      index = tile_index(x, y, z, msz, borig, bsz);
      flags = buf[index];
      TCOD_console_print_left(NULL, 0, 18, TCOD_BKGND_NONE, "%i, %i, %i", map_item_lit(flags), map_item_in_volume(flags), map_item_los(flags));
      tileIndex = map_tile_at(m, x, y, z);
      drawX = x*2+z*((msz.x*2)+1);
      drawY = y;
      //TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "%i", index);
      if(map_item_visible(flags)) {
         //visible and lit and in volume
         TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "%i", tileIndex);
      }
      // else if(!map_item_lit(flags) && map_item_in_volume(flags) && map_item_los(flags)) {
      //   //not lit and viewable
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "_");
      // }
      // else if(!map_item_lit(flags) && map_item_in_volume(flags) && !map_item_los(flags)) {
      //   //not lit and not los
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, ",");
      // }
      // else if(!map_item_lit(flags) && !map_item_in_volume(flags) && map_item_los(flags)) {
      //   //not lit and not in vol
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "d");
      // }
      // else if(map_item_lit(flags) && !map_item_in_volume(flags) && map_item_los(flags)) {
      //   //lit and in los, but not in vol
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "a");
      // }
      // else if(map_item_lit(flags) && map_item_in_volume(flags) && !map_item_los(flags)) {
      //   //lit and in vol, but not in los
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "b");
      // }
      // else if(map_item_lit(flags) && !map_item_in_volume(flags) && !map_item_los(flags)) {
      //   //lit and not in vol or los (or los wasn't checked)
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, ".");
      // }
      // else if(!map_item_lit(flags) && !map_item_in_volume(flags) && !map_item_los(flags)) { 
      //   //not lit, in vol, or in los
      //   TCOD_console_print_left(NULL, drawX, drawY, TCOD_BKGND_NONE, "x");
      // }
    }
  }
}

void draw_object(Stimulus st) {
  unsigned char visflags = stimulus_obj_sight_change_get_new_flags(st);
  mapVec pos = stimulus_obj_sight_change_get_position(st);
  char *id = stimulus_obj_sight_change_get_id(st);
  if(!map_item_visible(visflags)) {
    TCOD_console_print_left(NULL, pos.x*2, pos.y, TCOD_BKGND_NONE, "X");
  } else {
    TCOD_console_print_left(NULL, pos.x*2, pos.y, TCOD_BKGND_NONE, id);
  }
}

void drawstimuli(Map m, Sensor s) {
  TCOD_list_t stims = sensor_consume_stimuli(s);
  unsigned char *tiles;
  mapVec pos, size, oldPt, delta;
  unsigned char visflags;
  if(TCOD_list_size(stims) > 0) {
    TCOD_console_print_left(NULL, 0, 10, TCOD_BKGND_NONE, "                            ");
  }
  for(int i = 0; i < TCOD_list_size(stims); i++) {
    //this is a very naive approach that completely ignores the possibility of overdraw and 'forgets' object positions
    Stimulus st = TCOD_list_get(stims, i);
    stimtype type = stimulus_type(st);
    TCOD_console_print_left(NULL, i*2, 10, TCOD_BKGND_NONE, "s%i", type);
    switch(type) {
      case StimTileLitChange:
      case StimTileVisChange:
        //redraw all tiles
        tiles = stimulus_tile_sight_change_get_new_tiles(st);
        pos = stimulus_tile_sight_change_get_position(st);
        size = stimulus_tile_sight_change_get_size(st);
        drawtiles(m, tiles, s, pos, size);
        break;
      case StimObjLitChange:
      case StimObjVisChange:
        //redraw object
        draw_object(st);
        break;
      case StimObjMoved:
        visflags = stimulus_obj_sight_change_get_new_flags(st);
        pos = stimulus_obj_sight_change_get_position(st);
        delta = stimulus_obj_moved_get_dir(st);
        oldPt = mapvec_subtract(pos, delta);
        TCOD_console_print_left(NULL, oldPt.x*2, oldPt.y, TCOD_BKGND_NONE, "x");
        draw_object(st);
        TCOD_console_print_left(NULL, 0, 15, TCOD_BKGND_NONE, "got move");
        break;
      case StimGeneric:
      default:
        TCOD_console_print_left(NULL, i*9, 16, TCOD_BKGND_NONE, "generic %d", i);
        break;
    }
    stimulus_free(st);
  }
  TCOD_list_delete(stims);
}

void drawmap(Map m, Object o) {
  Sensor s;
  for(int i = 0; i < object_sensor_count(o); i++) {
    s = object_get_sensor(o, i);
    drawstimuli(m, s);
    TCOD_console_print_left(NULL, 0, 13+i, TCOD_BKGND_NONE, "<%f %f %f>", sensor_facing(s).x, sensor_facing(s).y, sensor_facing(s).z);
  }
}

int main( int argc, char *argv[] ) {
  char *font="tilesense/libtcod/fonts/courier12x12_aa_tc.png";
  int nb_char_horiz=0,nb_char_vertic=0;
  int font_flags=TCOD_FONT_TYPE_GREYSCALE|TCOD_FONT_LAYOUT_TCOD;
	TCOD_console_set_custom_font(font,font_flags,nb_char_horiz,nb_char_vertic);
	TCOD_console_init_root(80,40,"salamandeRL",false);
	
  Map room = initRoom();
  Object player = initPlayer(room);
  object_sense(player);
  
  TCOD_sys_set_fps(30);

  float elapsed = 0;
  char finished = 0;
	TCOD_key_t key = {TCODK_NONE,0};
	TCOD_console_set_foreground_color(NULL,TCOD_white);
	do {
	  key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
    if(key.vk != TCODK_NONE) {
      TCOD_console_clear(NULL);
    }
		
		TCOD_console_print_right(NULL,79,26,TCOD_BKGND_NONE,"last frame : %3d ms (%3d fps)", (int)(TCOD_sys_get_last_frame_length()*1000), TCOD_sys_get_fps());
		TCOD_console_print_right(NULL,79,27,TCOD_BKGND_NONE,"elapsed : %8dms %4.2fs", TCOD_sys_elapsed_milli(),TCOD_sys_elapsed_seconds());
		TCOD_console_print_left(NULL,0,27,TCOD_BKGND_NONE,"other stat stuff can go here");
		//map
    drawmap(room, player);
    TCOD_console_print_left(NULL, 
      object_position(player).x*2, object_position(player).y, 
      TCOD_BKGND_NONE,"@");
		//divider
		TCOD_console_print_left(NULL,0,28,TCOD_BKGND_NONE,
		  "--------------------------------------------------------------------------------"
		);
	  //text display
    TCOD_console_print_left(NULL,2,29,TCOD_BKGND_NONE,"we'll probably put text down here.");
		
		/* update the game screen */
		TCOD_console_flush();

    if(key.vk == TCODK_RIGHT) {
      map_turn_object(room, "@", 1);
    } else if(key.vk == TCODK_LEFT) {
      map_turn_object(room, "@", -1);
    } else if(key.vk == TCODK_CHAR) {
      switch(key.c) {
        case 'w':
          map_move_object(room, "@", (mapVec){0, -1, 0});
          break;
        case 'a':
          map_move_object(room, "@", (mapVec){-1, 0, 0});
          break;
        case 's':
          map_move_object(room, "@", (mapVec){0, 1, 0});
          break;
        case 'd':
          map_move_object(room, "@", (mapVec){1, 0, 0});
          break;
        case 'q':
          finished = 1;
          break;
        //next, handle chomping
        default:
          break;
  		}
    }

	} while (!finished && !TCOD_console_is_window_closed());
	return 0;
}


