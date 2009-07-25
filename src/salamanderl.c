#include <stdlib.h> /* for NULL */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <libtcod.h>

#include <tilesense.h>

#include "loader.h"
#include "drawinfo.h"
#include "moveinfo.h"
#include "tileinfo.h"
#include "objectinfo.h"
#include "scrollconsole.h"

#include "sobject.h"

void drawtiles(Map m, perception *buf, Sensor s, mapVec pos, mapVec size, TCOD_color_t sub) {
  int index=0;
  unsigned char tileIndex;
  perception flags;
  int drawX, drawY;
  Volume vol = sensor_volume(s);
  // mapVec borig, bsz;
  // volume_swept_bounds(vol, &borig, &bsz);
  mapVec msz = map_size(m);
  float ystart = CLIP(pos.y, 0, msz.y);
  float xstart = CLIP(pos.x, 0, msz.x);
  float yend = CLIP(pos.y+size.y, 0, msz.y);
  float xend = CLIP(pos.x+size.x, 0, msz.x);
//  float zstart = CLIP(pos.z, 0, msz.z);
//  float zend = CLIP(pos.z+size.z, 0, msz.z);
  float zstart = CLIP(sensor_position(s).z, 0, msz.z);
  float zend = zstart+1;
  Tile t;
  int x, y, z;
  TileInfo ti;
  TCOD_list_t drawInfos;
  for(z = zstart; z < zend; z++) {
    for(y = ystart; y < yend; y++) {
      for(x = xstart; x < xend; x++) {
        int belowZ = z;
        bool skip = false;
        do {
          index = tile_index(x, y, belowZ, msz, pos, size);
          flags = buf[index];
          tileIndex = map_tile_at(m, x, y, belowZ);
          skip = (tileIndex == 0); //later, skip if drawinfo's symbol is ' '?
          if(skip && (belowZ > 0) && (belowZ > pos.z)) { belowZ--; }
        } while(skip && (belowZ >= 0) && (belowZ >= pos.z));
        // drawX = x*2+z*((msz.x*2)+1);
        drawX = x*2;
        drawY = y;
        t = map_get_tile(m, tileIndex);
        ti = tile_context(t);
        drawInfos = tileinfo_drawinfos(ti);
        if(drawInfos) {
          //in los and lit and in volume
          DrawInfo di = drawinfo_get_z_level(drawInfos, z, belowZ);
          bool visible = false;
          if(z == belowZ) {
            visible = ((flags.edgelos > 1 || flags.surflos > 1)) && (flags.surfvol > 1) && (flags.surflit > 1);
          } else if(z > belowZ) {
            visible = ((flags.edgelos > 1 || flags.surflos > 1)) && (flags.surfvol > 1) && (flags.toplit > 1);
          }
          if(visible && di) {
            TCOD_console_set_back(NULL, drawX, drawY, TCOD_color_subtract(drawinfo_back_color(di), sub));
            TCOD_console_set_fore(NULL, drawX, drawY, TCOD_color_subtract(drawinfo_fore_color(di), sub));
            TCOD_console_set_char(NULL, drawX, drawY, drawinfo_symbol(di));
          }
        }
      }
    }
  }
}

void mem_remember(perception *mem, Map m, perception *tiles, mapVec pos, mapVec size) {
  mapVec msz = map_size(m);
  for(int x = pos.x; x < pos.x+size.x; x++) {
    for(int y = pos.y; y < pos.y+size.y; y++) {
      for(int z = pos.z; z < pos.z+size.z; z++) {
        if(tile_index_in_bounds(x, y, z, msz, pos, size) &&
           tile_index_in_bounds(x, y, z, msz, mapvec_zero, msz)) {
          int srcIndex = tile_index(x, y, z, msz, pos, size);
          int dstIndex = tile_index(x, y, z, msz, mapvec_zero, msz);
          #warning this should also look downwards to find tiles below like draw_tiles() does.
          if(map_item_visible(tiles[srcIndex])) {
            mem[dstIndex] = tiles[srcIndex];
          }
        }
      }
    }
  }
}

void mem_draw(perception *mem, Map m, TCOD_list_t sensors) {
  drawtiles(m, mem, TCOD_list_get(sensors, 0), mapvec_zero, map_size(m), (TCOD_color_t){90, 90, 90});
  TS_LIST_FOREACH(sensors, 
    mapVec borig;
    mapVec bsz;
    sensor_swept_bounds(each, &borig, &bsz);
    drawtiles(m, sensor_get_perceptmap(each), each, borig, bsz, (TCOD_color_t){0, 0, 0});
  );
}

void draw_objectinfo(ObjectInfo context, perception flags, int senseZ, mapVec pos, TCOD_color_t backAugment, TCOD_list_t drawnOIs) {
  TCOD_list_t drawInfos = objectinfo_drawinfos(context);
  DrawInfo di = drawinfo_get_z_level(drawInfos, senseZ, pos.z);

//if there are multiple objects at this pos this frame (figured through drawnOIs?), choose which one to draw based on the frame count or something

  if(!map_item_visible(flags)) {
//    TCOD_console_print_left(NULL, pos.x*2, pos.y, "X");
  } else if(!TCOD_list_contains(drawnOIs, context)) {
    int drawX = pos.x*2, drawY = pos.y;
    TCOD_console_set_back(NULL, drawX, drawY, TCOD_color_add(drawinfo_back_color(di), backAugment));
    TCOD_console_set_fore(NULL, drawX, drawY, drawinfo_fore_color(di));
    TCOD_console_set_char(NULL, drawX, drawY, drawinfo_symbol(di));
    TCOD_list_push(drawnOIs, context);
  }
}

void draw_object_stimulus(Sensor s, Stimulus st, TCOD_list_t drawnOIs) {
  perception flags = stimulus_obj_sight_change_get_new_perception(st);
  mapVec pos = stimulus_obj_sight_change_get_position(st);
  ObjectInfo context = stimulus_obj_sight_change_get_context(st);
  mapVec sensePos = sensor_position(s);
  draw_objectinfo(context, flags, sensePos.z, pos, (TCOD_color_t){255, 0, 0}, drawnOIs);
}

void draw_object(Sensor s, Object o, TCOD_list_t drawnOIs) {
  perception *map = sensor_get_perceptmap(s);
  mapVec borig, bsz;
  sensor_swept_bounds(s, &borig, &bsz);
  mapVec pos = object_position(o);
  mapVec msz = map_size(object_map(o));
  ObjectInfo context = object_context(o);
  mapVec sensePos = sensor_position(s);
  if(tile_index_in_bounds(pos.x, pos.y, pos.z, msz, borig, bsz)) {
    perception flags = map[tile_index(pos.x, pos.y, pos.z, msz, borig, bsz)];
    draw_objectinfo(context, flags, sensePos.z, pos, (TCOD_color_t){0,0,0}, drawnOIs);
  }
}

void drawstimuli(Map m, Sensor s, TCOD_list_t drawnOIs, perception *mem, ScrollConsole msgConsole) {
  TCOD_list_t stims = sensor_consume_stimuli(s);
  perception *tiles;
  mapVec pos, size, oldPt, delta;
  char *msg;
  perception visflags;
  for(int i = 0; i < TCOD_list_size(stims); i++) {
    //this is a very naive approach that completely ignores the possibility of overdraw and 'forgets' object positions
    Stimulus st = TCOD_list_get(stims, i);
    stimtype type = stimulus_type(st);
    // TCOD_console_print_left(NULL, i*2, 10, "s%i", type);
    switch(type) {
      case StimTileLitChange:
      case StimTileVisChange:
        //redraw all tiles -- actually, we're also going to do that separately for map memory
        tiles = stimulus_tile_sight_change_get_new_perceptmap(st);
        pos = stimulus_tile_sight_change_get_position(st);
        size = stimulus_tile_sight_change_get_size(st);
        mem_remember(mem, m, tiles, pos, size);
        drawtiles(m, tiles, s, pos, size, (TCOD_color_t){0,0,0});
        break;
      case StimObjLitChange:
      case StimObjVisChange:
        //redraw object
        draw_object_stimulus(s, st, drawnOIs);
        break;
      case StimObjMoved:
        visflags = stimulus_obj_sight_change_get_new_perception(st);
        pos = stimulus_obj_sight_change_get_position(st);
        delta = stimulus_obj_moved_get_dir(st);
        oldPt = mapvec_subtract(pos, delta);
        // TCOD_console_print_left(NULL, oldPt.x*2, oldPt.y, "x");
        draw_object_stimulus(s, st, drawnOIs);
        // TCOD_console_print_left(NULL, 0, 15, "got move");
        break;
      case SalMessage:
        msg = stimulus_generic_get_context(st);
        scrollconsole_push(msgConsole, msg);
        free(msg);
        break;
      case StimGeneric:
      default:
        // TCOD_console_print_left(NULL, i*9, 16, "generic %d", i);
        break;
    }
    stimulus_free(st);
  }
  TCOD_list_delete(stims);
}

void drawmap(Map m, Object o, TCOD_list_t drawnOIs, perception *mem, ScrollConsole msgConsole) {
  TCOD_console_set_background_flag(NULL, TCOD_COLOROP_SET);
  Sensor s;
  //draw tiles from memory
  mem_draw(mem, m, object_sensors(o));
  for(int i = 0; i < object_sensor_count(o); i++) {
    s = object_get_sensor(o, i);
    drawstimuli(m, s, drawnOIs, mem, msgConsole);
    TCOD_list_t visible = sensor_get_visible_objects(s);
    TS_LIST_FOREACH(visible,
      draw_object(s, each, drawnOIs);
    );
    //hack to ensure player is drawn on top of other objects
    if(TCOD_list_contains(drawnOIs, object_context(o))) {
      TCOD_list_remove(drawnOIs, object_context(o));
      draw_object(s, o, drawnOIs);
    }
    // TCOD_console_print_left(NULL, 0, 13+i, "<%f %f %f>", sensor_facing(s).x, sensor_facing(s).y, sensor_facing(s).z);
  }
  TCOD_list_clear(drawnOIs);
  TCOD_console_set_foreground_color(NULL, (TCOD_color_t){255, 255, 255});
  TCOD_console_set_background_color(NULL, (TCOD_color_t){0, 0, 0});
}

int main( int argc, char *argv[] ) {
  char *font="tilesense/libtcod/fonts/courier12x12_aa_tc.png";
  int nb_char_horiz=0,nb_char_vertic=0;
  int font_flags=TCOD_FONT_TYPE_GREYSCALE|TCOD_FONT_LAYOUT_TCOD;
	TCOD_console_set_custom_font(font,font_flags,nb_char_horiz,nb_char_vertic);
	TCOD_console_init_root(80,40,"salamandeRL",false);
  ScrollConsole descConsole = scrollconsole_init(scrollconsole_new(), 80, 11);
	
  Loader loader = loader_init(loader_new(), "rsrc");
  loader_load_save(loader, "start");
  Map map = loader_get_map(loader, "cage");
  Object player = map_get_object_named(map, "player");
  
  mapVec msz = map_size(map);
  perception *mem = calloc(msz.x*msz.y*msz.z, sizeof(perception));

  object_sense(player);
  
  TCOD_sys_set_fps(10);
  TCOD_list_t drawnOIs = TCOD_list_new();
  
  char finished = 0;
  float dt;
	TCOD_key_t key = {TCODK_NONE,0};
	TCOD_console_set_foreground_color(NULL,TCOD_white);
	do {
	  key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED | TCOD_KEY_RELEASED);
    if(key.pressed && (key.vk == TCODK_CHAR) && (key.c == 'q')) {
      finished = 1;
      break;
    }
    TCOD_console_clear(NULL);
		
		TCOD_console_print_right(NULL,79,26,"last frame : %3d ms (%3d fps)", (int)(TCOD_sys_get_last_frame_length()*1000), TCOD_sys_get_fps());
		TCOD_console_print_right(NULL,79,27,"elapsed : %8dms %4.2fs", TCOD_sys_elapsed_milli(),TCOD_sys_elapsed_seconds());
		TCOD_console_print_left(NULL,0,27,"other stat stuff can go here");
    
    dt = TCOD_sys_get_last_frame_length();
    sobject_input(player, key, dt);
    sobject_update(player, dt);
    
		//map
    drawmap(map, player, drawnOIs, mem, descConsole);
		TCOD_console_print_left(NULL,0,28,
		  "--------------------------------------------------------------------------------"
		);
	  //text display
    scrollconsole_blit(descConsole, NULL, 0, 29, 255);
    
    /* update the game screen */
		TCOD_console_flush();

	} while (!finished && !TCOD_console_is_window_closed());
  scrollconsole_free(descConsole);
	return 0;
}


