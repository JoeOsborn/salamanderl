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

void drawtiles(Map m, perception *buf, Sensor s, mapVec pos, mapVec size) {
  int index=0;
  unsigned char tileIndex;
  perception flags;
  int drawX, drawY;
  Volume vol = sensor_volume(s);
  mapVec borig, bsz;
  volume_swept_bounds(vol, &borig, &bsz);
  mapVec msz = map_size(m);
  float ystart = CLIP(pos.y, 0, msz.y);
  float xstart = CLIP(pos.x, 0, msz.x);
  float yend = CLIP(pos.y+size.y, 0, msz.y);
  float xend = CLIP(pos.x+size.x, 0, msz.x);
//  float zstart = CLIP(pos.z, 0, msz.z);
//  float zend = CLIP(pos.z+size.z, 0, msz.z);
  float zstart = CLIP(volume_position(vol).z, 0, msz.z);
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
          index = tile_index(x, y, belowZ, size, borig, bsz);
          flags = buf[index];
          tileIndex = map_tile_at(m, x, y, belowZ);
          skip = (tileIndex == 0); //later, skip if drawinfo's symbol is ' '?
          if(skip && (belowZ > 0) && (belowZ > borig.z)) { belowZ--; }
        } while(skip && (belowZ >= 0) && (belowZ >= borig.z));
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
            TCOD_console_set_foreground_color(NULL, drawinfo_fore_color(di));
            TCOD_console_set_background_color(NULL, drawinfo_back_color(di));
            TCOD_console_print_left(NULL, drawX, drawY, "%c", drawinfo_symbol(di));
          }
        }
      }
    }
  }
}

void draw_object(Sensor s, Stimulus st) {
  perception flags = stimulus_obj_sight_change_get_new_perception(st);
  mapVec pos = stimulus_obj_sight_change_get_position(st);
  ObjectInfo context = stimulus_obj_sight_change_get_context(st);
  mapVec sensePos = sensor_position(s);
  
  TCOD_list_t drawInfos = objectinfo_drawinfos(context);
  DrawInfo di = drawinfo_get_z_level(drawInfos, sensePos.z, pos.z);
    
  if(!map_item_visible(flags)) {
//    TCOD_console_print_left(NULL, pos.x*2, pos.y, "X");
  } else {
    TCOD_console_set_foreground_color(NULL, drawinfo_fore_color(di));
    TCOD_console_set_background_color(NULL, drawinfo_back_color(di));
    TCOD_console_print_left(NULL, pos.x*2, pos.y, "%c", drawinfo_symbol(di));
  }
}

void drawstimuli(Map m, Sensor s) {
  TCOD_list_t stims = sensor_consume_stimuli(s);
  perception *tiles;
  mapVec pos, size, oldPt, delta;
  perception visflags;
  for(int i = 0; i < TCOD_list_size(stims); i++) {
    //this is a very naive approach that completely ignores the possibility of overdraw and 'forgets' object positions
    Stimulus st = TCOD_list_get(stims, i);
    stimtype type = stimulus_type(st);
    // TCOD_console_print_left(NULL, i*2, 10, "s%i", type);
    switch(type) {
      case StimTileLitChange:
      case StimTileVisChange:
        //redraw all tiles
        tiles = stimulus_tile_sight_change_get_new_perceptmap(st);
        pos = stimulus_tile_sight_change_get_position(st);
        size = stimulus_tile_sight_change_get_size(st);
        drawtiles(m, tiles, s, pos, size);
        break;
      case StimObjLitChange:
      case StimObjVisChange:
        //redraw object
        draw_object(s, st);
        break;
      case StimObjMoved:
        visflags = stimulus_obj_sight_change_get_new_perception(st);
        pos = stimulus_obj_sight_change_get_position(st);
        delta = stimulus_obj_moved_get_dir(st);
        oldPt = mapvec_subtract(pos, delta);
        // TCOD_console_print_left(NULL, oldPt.x*2, oldPt.y, "x");
        draw_object(s, st);
        // TCOD_console_print_left(NULL, 0, 15, "got move");
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

void drawmap(Map m, Object o) {
  Sensor s;
  for(int i = 0; i < object_sensor_count(o); i++) {
    s = object_get_sensor(o, i);
    drawstimuli(m, s);
    // TCOD_console_print_left(NULL, 0, 13+i, "<%f %f %f>", sensor_facing(s).x, sensor_facing(s).y, sensor_facing(s).z);
  }
  TCOD_console_set_foreground_color(NULL, (TCOD_color_t){255, 255, 255});
  TCOD_console_set_background_color(NULL, (TCOD_color_t){0, 0, 0});
}

bool smap_turn_object(Map map, char *obj, int amt) {
  map_turn_object(map, obj, amt);
  return true;
}

/*
when falling, the top pit gets on_enter, then fall_through, then on_exit.
the pit below that gets fall_into, on_enter, fall_through, on_exit
this repeats until:
  floor: fall_into, on_enter
  wall: fall_onto, atop
  pit at z=0: fall_into, on_enter
when walking normally, the old tile gets on_exit and the new gets on_enter
when walking up stairs, the old tile gets on_exit, the stairs get on_enter, on_walk_up, on_exit, on_atop, the square above the stairs gets on_enter
when walking down stairs, the old tile gets on_exit, the tile above the stairs gets on_enter, on_exit, the stairs get on_walk_down, on_enter.
when walking into a wall, only the wall's on_bump is triggered

this info might be good to have in a diagram
*/

bool smap_move_object(Map map, char *obj, mapVec amt) {
  //get the destination tile's tileinfo
  //get the object's objectinfo
  Object o = map_get_object_named(map, obj);
  mapVec newPos = mapvec_add(object_position(o), amt);
  mapVec firstPos=object_position(o);
  TileInfo ti = tile_context(map_tiledef_at(map, newPos.x, newPos.y, newPos.z));
  TileInfo belowTi=NULL, aboveTi=NULL;
  TileInfo firstTi=NULL;
  ObjectInfo oi = object_context(o);
  //is it a pit? if so, move the player there and down a z level.
  //repeat until a tile is reached.
  MoveInfo mi = objectinfo_moveinfo(oi);
  firstTi = tile_context(map_tiledef_at(map, firstPos.x, firstPos.y, firstPos.z));
  if(tileinfo_is_pit(ti)) {
    tileinfo_trigger(firstTi, o, "on_exit");
    map_move_object(map, obj, amt);
    tileinfo_trigger(ti, o, "on_enter");
    if(newPos.z > 0) {
      belowTi = tile_context(map_tiledef_at(map, newPos.x, newPos.y, newPos.z-1));
    } else {
      //z=0 -- don't even bother to do anything
      return true; //we're done, we've hit rock bottom
    }
    //if there's only one level of pit or there's a wall below, drop firstTi for the stairs check
    if(tileinfo_is_pit(firstTi) && firstPos.z > 0) {
      firstTi = tile_context(map_tiledef_at(map, firstPos.x, firstPos.y, firstPos.z-1));
    }
    while(tileinfo_is_pit(belowTi) || tileinfo_moveinfo_can_enter(belowTi, mi)) { 
      //first off, bail if the below is a stairs and the player was on a stairs before (and hasn't already fallen some). no need to go down the stairs in that case.
      if(tileinfo_is_stairs(belowTi) && (firstPos.z == newPos.z) && tileinfo_is_stairs(firstTi)) {
        tileinfo_trigger(belowTi, o, "on_atop");
        break;
      }
      amt = (mapVec){0, 0, -1};
      newPos.z--;
      if(!tileinfo_is_stairs(belowTi) && (firstPos.z == (newPos.z+1))) {
        tileinfo_trigger(ti, o, "on_fall_through");
      }
      tileinfo_trigger(ti, o, "on_exit");

      map_move_object(map, obj, amt);

      if(tileinfo_is_stairs(belowTi) && (firstPos.z == (newPos.z+1))) {
        //we must be going down these stairs then
        tileinfo_trigger(belowTi, o, "on_walk_down");
      } else {
        //otherwise we stumbled down
        tileinfo_trigger(belowTi, o, "on_fall_into");
      }
      tileinfo_trigger(belowTi, o, "on_enter");
      ti = belowTi;
      if(newPos.z > 0) {
        belowTi = tile_context(map_tiledef_at(map, newPos.x, newPos.y, newPos.z-1));
      } else {
        break; //we're done, we've hit rock bottom
      }
      if(tileinfo_is_pit(ti) && !tileinfo_moveinfo_can_enter(belowTi, mi)) {
        //must be a wall we're on top of
        if(firstPos.z > (newPos.z+1)) {
          //we fell a long way
          tileinfo_trigger(belowTi, o, "on_fall_onto");
        }
        tileinfo_trigger(belowTi, o, "on_atop");
      }
    }
    return true;
  } else if(tileinfo_moveinfo_can_enter(ti, mi)) {
    //is it a stairway?
    tileinfo_trigger(firstTi, o, "on_exit");
    map_move_object(map, obj, amt);
    tileinfo_trigger(ti, o, "on_enter");
    //we just entered the tile
    if(tileinfo_is_stairs(ti)) {
    //if the player's z is the same as this tile's z and he's not standing on a stairs and the tile above is enterable, move player z up to z+1. 
    //[no need for the position check since the other case is caught by the pitfall.]
//      if(objectPosition.z == newPos.z) { //stairs up
        //no need to go up if the old tile is stairs or if the new tile is non-enterable
      if(newPos.z < (map_size(map).z-1)) {
        aboveTi = tile_context(map_tiledef_at(map, newPos.x, newPos.y, newPos.z+1));
      } else {
        return true; //we're done, we've hit rock bottom
      }
        if(!tileinfo_is_stairs(firstTi) && tileinfo_moveinfo_can_enter(aboveTi, mi)) {
          tileinfo_trigger(ti, o, "on_walk_up");
          tileinfo_trigger(ti, o, "on_exit");          
          map_move_object(map, obj, (mapVec){0, 0, 1});
          tileinfo_trigger(aboveTi, o, "on_enter");
          newPos.z++;
        }
//      }
    //if the player's z is higher than this tile's z and he's not standing on a stairs, move him down to this z. ((this is already handled by pit-dropping.))
    }
    return true;
  } else {
    tileinfo_trigger(ti, o, "on_bump");
    return false;
  }
}

//next steps: object memory model, introduce chomping.
//it's okay to hard-code these for now.

int main( int argc, char *argv[] ) {
  char *font="tilesense/libtcod/fonts/courier12x12_aa_tc.png";
  int nb_char_horiz=0,nb_char_vertic=0;
  int font_flags=TCOD_FONT_TYPE_GREYSCALE|TCOD_FONT_LAYOUT_TCOD;
	TCOD_console_set_custom_font(font,font_flags,nb_char_horiz,nb_char_vertic);
	TCOD_console_init_root(80,40,"salamandeRL",false);
	
  Loader loader = loader_init(loader_new(), "rsrc");
  loader_load_save(loader, "start");
  Map map = loader_get_map(loader, "cage");
  Object player = map_get_object_named(map, "player");

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
		
		TCOD_console_print_right(NULL,79,26,"last frame : %3d ms (%3d fps)", (int)(TCOD_sys_get_last_frame_length()*1000), TCOD_sys_get_fps());
		TCOD_console_print_right(NULL,79,27,"elapsed : %8dms %4.2fs", TCOD_sys_elapsed_milli(),TCOD_sys_elapsed_seconds());
		TCOD_console_print_left(NULL,0,27,"other stat stuff can go here");
		//map
    drawmap(map, player);
    DrawInfo pdi = TCOD_list_get(objectinfo_drawinfos(object_context(player)), 0);
    TCOD_console_print_left(NULL, object_position(player).x*2, object_position(player).y, "%c", drawinfo_symbol(pdi));
		//divider
		TCOD_console_print_left(NULL,0,28,
		  "--------------------------------------------------------------------------------"
		);
	  //text display
    TCOD_console_print_left(NULL,2,29,"we'll probably put text down here.");
		
		/* update the game screen */
		TCOD_console_flush();

    if(key.vk == TCODK_RIGHT) {
      smap_turn_object(map, "player", 1);
    } else if(key.vk == TCODK_LEFT) {
      smap_turn_object(map, "player", -1);
    } else if(key.vk == TCODK_UP) {
      smap_move_object(map, "player", (mapVec){0, 0,  1});
    } else if(key.vk == TCODK_DOWN) {
      smap_move_object(map, "player", (mapVec){0, 0, -1});
    } else if(key.vk == TCODK_CHAR) {
      switch(key.c) {
        case 'w':
          smap_move_object(map, "player", (mapVec){0, -1, 0});
          break;
        case 'a':
          smap_move_object(map, "player", (mapVec){-1, 0, 0});
          break;
        case 's':
          smap_move_object(map, "player", (mapVec){0, 1, 0});
          break;
        case 'd':
          smap_move_object(map, "player", (mapVec){1, 0, 0});
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


