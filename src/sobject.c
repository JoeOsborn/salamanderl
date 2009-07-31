#include "sobject.h"

#include "moveinfo.h"
#include "tileinfo.h"
#include "objectinfo.h"

#define TUG(_tugger, _tugged, _part, _cmp, _dir) do {                         \
  if(amt._part _cmp 0) {                                                      \
    objectinfo_trigger(object_context(_tugged), _tugged, _tugger, "on_tug_" #_dir);  \
  }                                                                           \
} while(0)

void sobject_raw_move(Object o, mapVec amt) {
  ObjectInfo oi = object_context(o);
  Object o2 = objectinfo_attached_object(oi);
  AttachMode mode = objectinfo_attach_mode(oi);
  if(o2) {
    if(mode == AttachCarry) {
      map_move_object(object_map(o), object_id(o), amt);
      map_move_object(object_map(o), object_id(o2), amt);
    } else if(mode == AttachLatch) {
      TUG(o, o2, x, <, left);
      TUG(o, o2, x, >, right);
      TUG(o, o2, y, <, back);
      TUG(o, o2, y, >, forward);
    }
  } else {
    map_move_object(object_map(o), object_id(o), amt);
  }
}
#undef TUG

void sobject_chomp(Object o) {
  ObjectInfo oi = object_context(o);
  if(objectinfo_chomping(oi)) { return; }
  Map map = object_map(o);
  mapVec position = object_position(o);
  TCOD_list_t objectsAtTile = map_objects_at_position(map, position);
  if(!objectsAtTile) { return; }
  TS_LIST_FOREACH(objectsAtTile, if(each != o) { 
    objectinfo_trigger(object_context(each), each, o, "on_chomp");
    if(objectinfo_chomping(oi)) { return; }
  });
  //send "nothing to chomp"
}

void sobject_unchomp(Object o) {
  ObjectInfo oi = object_context(o);
  if(!objectinfo_chomping(oi)) { return; }
  Object other = objectinfo_attached_object(object_context(o));
  if(other) {
    objectinfo_trigger(object_context(other), other, o, "on_unchomp");
  }
}

void sobject_presence_trigger(Object o, mapVec position, char *trig) {
  ObjectInfo oi = object_context(o);
  Object o2 = objectinfo_attached_object(oi);
  if(o2 && (objectinfo_attach_mode(oi) == AttachCarry)) {
    #warning looped attachment is deadly
    sobject_presence_trigger(o2, position, trig);
  }
  Map map = object_map(o);
  //trigger on_inside for the player's current tile
  Tile t = map_tiledef_at_position(map, position);
  TileInfo ti = tile_context(t);
  tileinfo_trigger(ti, t, o, trig);
  //and for any objects in that tile
  TCOD_list_t objectsAtTile = map_objects_at_position(map, position);
  if(!objectsAtTile) { return; }
  TS_LIST_FOREACH(objectsAtTile, 
    if(each != o && (each != objectinfo_attached_object(oi))) { 
      objectinfo_trigger(object_context(each), each, o, trig); 
    } 
  );
}

bool sobject_turn(Object o, int amt) {
  Map map = object_map(o);
  map_turn_object(map, object_id(o), amt);
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

void sobject_fall(Object o) {
  ObjectInfo oi = object_context(o);
  if(!objectinfo_falling(oi)) { return; }
  Map map = object_map(o);
  mapVec newPos = object_position(o);
  mapVec belowPos=(mapVec){newPos.x, newPos.y, newPos.z-1};
  TileInfo ti = tile_context(map_tiledef_at_position(map, newPos));
  TileInfo belowTi = tile_context(map_tiledef_at_position(map, belowPos));
  //is it a pit? if so, move the player there and down a z level.
  //repeat until a tile is reached.
  MoveInfo mi = objectinfo_moveinfo(oi);
  if(tileinfo_is_pit(ti)) {
    if(tileinfo_is_pit(belowTi) || (tileinfo_moveinfo_can_enter(belowTi, mi) && !(tileinfo_is_stairs(belowTi)))) { 
      sobject_presence_trigger(o, newPos, "on_fall_through");
      sobject_presence_trigger(o, newPos, "on_exit");

      belowPos.z--;
      newPos.z--;

      sobject_raw_move(o, (mapVec){0, 0, -1});
        
      sobject_presence_trigger(o, newPos, "on_fall_into");
      sobject_presence_trigger(o, newPos, "on_enter");
      sobject_presence_trigger(o, newPos, "on_inside");
      ti = belowTi;
      if(newPos.z > 0) {
        belowTi = tile_context(map_tiledef_at(map, newPos.x, newPos.y, newPos.z-1));
      } else {
        objectinfo_set_falling(oi, false);
        //we're done, we've hit rock bottom
      }
      if(tileinfo_is_pit(ti) && (!tileinfo_moveinfo_can_enter(belowTi, mi) || tileinfo_is_stairs(belowTi))) {
        //must be a wall we're on top of
        objectinfo_set_falling(oi, false);
        sobject_presence_trigger(o, belowPos, "on_atop");
      }
      if(!tileinfo_is_pit(ti)) {
        objectinfo_set_falling(oi, false);
        sobject_presence_trigger(o, belowPos, "on_atop");
      }
    } else {
      objectinfo_set_falling(oi, false);
      sobject_presence_trigger(o, belowPos, "on_fall_onto");
    }
  } else {
    objectinfo_set_falling(oi, false);
  }
}

bool sobject_move(Object o, mapVec amt) {
  //get the destination tile's tileinfo
  //get the object's objectinfo
  ObjectInfo oi = object_context(o);
  Map map = object_map(o);
  mapVec newPos = mapvec_add(object_position(o), amt);
  mapVec belowPos=(mapVec){newPos.x, newPos.y, newPos.z-1};
  mapVec firstPos=object_position(o);
  TileInfo ti = tile_context(map_tiledef_at_position(map, newPos));
  if((amt.z != 0) && !objectinfo_underwater(oi)) { return false; } 
  TileInfo belowTi=NULL, aboveTi=NULL;
  TileInfo firstTi=NULL;
  //is it a pit? if so, move the player there and down a z level.
  //repeat until a tile is reached.
  MoveInfo mi = objectinfo_moveinfo(oi);
  firstTi = tile_context(map_tiledef_at_position(map, firstPos));
  if(tileinfo_moveinfo_can_enter(ti, mi)) {
    sobject_presence_trigger(o, firstPos, "on_exit");
    sobject_raw_move(o, amt);
    sobject_presence_trigger(o, newPos, "on_enter");
    if(tileinfo_is_pit(ti)) {
      if(newPos.z <= 0) {
        //bottom of the map, just bail out
        objectinfo_set_falling(oi, false);
        return true;
      }
      belowTi = tile_context(map_tiledef_at_position(map, belowPos));
      if(!tileinfo_is_pit(belowTi) && !tileinfo_moveinfo_can_enter(belowTi, mi)) {
        //must be something we've just stepped onto
        objectinfo_set_falling(oi, false);
        sobject_presence_trigger(o, belowPos, "on_atop");
        return true;
      }
      //if there's only one level of pit or there's a wall below, drop firstTi for the stairs check
      if(tileinfo_is_pit(firstTi) && firstPos.z > 0) {
        firstTi = tile_context(map_tiledef_at(map, firstPos.x, firstPos.y, firstPos.z-1));
      }
      //bail if the below is a stairs and the player was on a stairs before (and hasn't already fallen some). no need to go down the stairs in that case.
      if(tileinfo_is_stairs(belowTi) && (firstPos.z == newPos.z) && (newPos.z > 0)) {
        if(tileinfo_is_stairs(firstTi)) {
          objectinfo_set_falling(oi, false);
          sobject_presence_trigger(o, belowPos, "on_atop");
        } else {
          //go down the stairs
          objectinfo_set_falling(oi, false);
          sobject_presence_trigger(o, newPos, "on_inside");
          sobject_presence_trigger(o, newPos, "on_exit");
          sobject_raw_move(o, (mapVec){0, 0, -1});
          sobject_presence_trigger(o, belowPos, "on_enter");
          sobject_presence_trigger(o, belowPos, "on_walk_down");
        }
        return true;
      }
      objectinfo_set_falling(oi, true);
      return true;
    } else {
      objectinfo_set_falling(oi, false);
      //we just entered the tile
      if(tileinfo_is_stairs(ti)) {
      //if the player's z is the same as this tile's z and he's not standing on a stairs and the tile above is enterable, move player z up to z+1. 
      //[no need for the position check since the other case is caught by the pitfall.]
        if(firstPos.z == newPos.z) { //stairs up
        //no need to go up if the old tile is stairs or if the new tile is non-enterable
          mapVec abovePos = (mapVec){newPos.x, newPos.y, newPos.z+1};
          if(newPos.z < (map_size(map).z-1)) {
            aboveTi = tile_context(map_tiledef_at_position(map, abovePos));
          } else {
            return true; //we're done, we've hit the top
          }
          if(!tileinfo_is_stairs(firstTi) && tileinfo_moveinfo_can_enter(aboveTi, mi)) {
            sobject_presence_trigger(o, newPos, "on_inside");
            sobject_presence_trigger(o, newPos, "on_walk_up");
            sobject_presence_trigger(o, newPos, "on_exit");
            #warning broken now, viewport not moving -- are we falling immediately?
            sobject_raw_move(o, (mapVec){0, 0, 1});
            sobject_presence_trigger(o, abovePos, "on_enter");
            newPos.z++;
          }
        }
      //if the player's z is higher than this tile's z and he's not standing on a stairs, move him down to this z. ((this is already handled by pit-dropping.))
      }
      return true;
    }
  } else {
    sobject_presence_trigger(o, newPos, "on_bump");
    return false;
  }
}

void sobject_input(Object player, TCOD_key_t key, float dt) {
  if(key.pressed) {
    if(key.vk == TCODK_SPACE) {
      sobject_chomp(player);
    }
    if(key.vk == TCODK_CHAR) {
      switch(key.c) {
        case 'w':
          sobject_move(player, (mapVec){0, -1, 0});
          break;
        case 'a':
          sobject_move(player, (mapVec){-1, 0, 0});
          break;
        case 's':
          sobject_move(player, (mapVec){0, 1, 0});
          break;
        case 'd':
          sobject_move(player, (mapVec){1, 0, 0});
          break;
        default:
          break;
  		}
    }
    if(key.vk == TCODK_RIGHT) {
      sobject_turn(player, 1);
    } else if(key.vk == TCODK_LEFT) {
      sobject_turn(player, -1);
    } else if(key.vk == TCODK_UP) {
      sobject_move(player, (mapVec){0, 0,  1});
    } else if(key.vk == TCODK_DOWN) {
      sobject_move(player, (mapVec){0, 0, -1});
    }
  } else {
    if(key.vk == TCODK_SPACE) {
      sobject_unchomp(player);
    }
  }
}
void sobject_update(Object player, float dt) {
  sobject_presence_trigger(player, object_position(player), "on_inside");
  ObjectInfo oi = object_context(player);
  Object o2 = objectinfo_attached_object(oi);
  AttachMode mode = objectinfo_attach_mode(oi);
  if(o2) {
    if(mode == AttachCarry) {
      objectinfo_trigger(object_context(player), player, o2, "on_carry");
      objectinfo_trigger(object_context(o2), o2, player, "on_carry");
    } else if(mode == AttachLatch) {
      objectinfo_trigger(object_context(player), player, o2, "on_latch");
      objectinfo_trigger(object_context(o2), o2, player, "on_latch");
    }
  } else {
    objectinfo_set_chomping(oi, false);
  }
  sobject_fall(player);
}