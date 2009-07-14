#include "loader/loaderlistener.h"

#include "drawinfo.h"
#include "moveinfo.h"
#include "tileinfo.h"

#include <tilesense.h>

DrawInfo drawinfo_init_structrecord(DrawInfo di, StructRecord sr, int index, int *finalZ) {
  int z = structrecord_get_prop_value_default(sr, "z", (TCOD_value_t)(index)).i;
  TCOD_color_t fore = structrecord_get_prop_value_default(sr, "fore", (TCOD_value_t)((TCOD_color_t){255, 255, 255})).col;
  TCOD_color_t back = structrecord_get_prop_value_default(sr, "back", (TCOD_value_t)((TCOD_color_t){0, 0, 0})).col;
  unsigned char symbol = structrecord_get_prop_value_default(sr, "symbol", (TCOD_value_t)' ').c;
  if(finalZ) {
    *finalZ = z;
  }
  return drawinfo_init(di, z, fore, back, symbol);
}

MoveInfo moveinfo_init_structrecord(MoveInfo mi, StructRecord sr) {
  TCOD_list_t props = structrecord_props(sr);
  TCOD_list_t flags = TCOD_list_new();
  for(int i = 0; i < TCOD_list_size(props); i++) {
    Prop p = TCOD_list_get(props, i);
    TCOD_list_push(flags, moveflag_init(moveflag_new(), prop_name(p), prop_value(p).b));    
  }
  return moveinfo_init(mi, flags);
}

Tile tile_init_structrecord(Tile t, StructRecord sr) {
  TCOD_list_t drawInfos = TCOD_list_new();
  TCOD_list_t moveInfos = TCOD_list_new();
  int drawIndex = 0;
  for(int i = 0; i < TCOD_list_size(structrecord_children(sr)); i++) {
    StructRecord kid = TCOD_list_get(structrecord_children(sr), i);
    if(strcmp(structrecord_type(kid), "draw") == 0) {
      TCOD_list_push(drawInfos, drawinfo_init_structrecord(drawinfo_new(), kid, drawIndex, &drawIndex));
      drawIndex++;
    } else if(strcmp(structrecord_type(kid), "movement") == 0) {
      TCOD_list_push(moveInfos, moveinfo_init_structrecord(moveinfo_new(), kid));
    }
  }
  bool moveDefaultAllowed = true;
  if(structrecord_has_prop(sr, "movement_default")) {
    moveDefaultAllowed = strcmp(
      structrecord_get_prop_value(sr, "movement_default").s, 
      "allow"
    ) == 0;
  }
  bool wallTransp=true, floorTransp=false, ceilTransp=true;
  #warning later, see if we can bring back fractional opacity.
  //it would be a postprocessing step after fov check.
  //use base first
  if(structrecord_has_prop(sr, "opacity")) {
    TCOD_list_t opacityFields = structrecord_get_prop_value(sr, "opacity").list;
    wallTransp = !((unsigned int)TCOD_list_get(opacityFields, 0));
    floorTransp = !((unsigned int)TCOD_list_get(opacityFields, 1));
    ceilTransp = !((unsigned int)TCOD_list_get(opacityFields, 2));
  }
  //then use any shorthands
  if(structrecord_has_prop(sr, "uniform_opacity")) {
    char uniformOpacity = structrecord_get_prop_value(sr, "uniform_opacity").c;
    wallTransp = floorTransp = ceilTransp = !uniformOpacity;
  }
  if(structrecord_has_prop(sr, "wall_opacity")) {
    char wallOpacity = structrecord_get_prop_value(sr, "wall_opacity").c;
    wallTransp = !wallOpacity;
  }
  if(structrecord_has_prop(sr, "floor_opacity")) {
    char floorOpacity = structrecord_get_prop_value(sr, "floor_opacity").c;
    floorTransp = !floorOpacity;
  }
  if(structrecord_has_prop(sr, "ceiling_opacity")) {
    char ceilingOpacity = structrecord_get_prop_value(sr, "ceiling_opacity").c;
    ceilTransp = !ceilingOpacity;
  }
  #warning moveinfo categories for up/down should exist
  //the default, however, should just be to treat the flag 'stairs' as meaning
  //bidirectional and automatic when entered.
  //for now, we'll just use a boolean for 'stairs'
  bool stairs = false;
  if(structrecord_is_flag_set(sr, "stairs")) {
    stairs = true;
  }
  bool pit = false;
  if(structrecord_is_flag_set(sr, "pit")) {
    pit = true;
  }
  #warning descs are being ignored
  TileInfo ti = tileinfo_init(tileinfo_new(), drawInfos, moveInfos, moveDefaultAllowed, stairs, pit);
  return tile_init(t, wallTransp, floorTransp, ceilTransp, ti);
}

Map map_init_structrecord(Map m, StructRecord sr) {
  //dimensions, ambient light, tilemap
  //implement these search methods
  int ambientLight = structrecord_get_prop_value(sr, "ambient_light").i;
  TCOD_list_t dimensions = structrecord_get_prop_value(sr, "dimensions").list;
  //make mapSize from floats in dimensions
  int sx = (int)TCOD_list_get(dimensions, 0);
  int sy = (int)TCOD_list_get(dimensions, 1);
  int sz = (int)TCOD_list_get(dimensions, 2);
  mapVec mapSize = (mapVec){sx, sy, sz};
  TCOD_list_t tileIndexList = structrecord_get_prop_value(sr, "tilemap").list;
  //make tileMap from the TCOD_list_t in "tilemap"
  unsigned char *tileMap = calloc(TCOD_list_size(tileIndexList), sizeof(unsigned short));
  for(int i = 0; i < TCOD_list_size(tileIndexList); i++) {
    void *tileID = TCOD_list_get(tileIndexList, i);
    tileMap[i] = *(unsigned char*)(&tileID);
  }
  //make a mapCtx that stores other info.  guess it can be null for now
  void *mapCtx = NULL;
  //also, don't know what to do just yet about null-tile context. null for now!
  void *blankTileCtx = NULL;
  return map_init(map_new(), structrecord_name(sr), mapSize, tileMap, ambientLight, blankTileCtx, mapCtx);
}

MapListener maplistener_new() {
  return calloc(1, sizeof(struct _maplistener));
}
MapListener maplistener_init(MapListener l, Loader loader) {
  l->loader = loader;
  l->workingStruct = NULL;
  l->tiles = TCOD_list_new();
  return l;
}
void maplistener_free(MapListener l) {
  StructRecord sr = l->workingStruct;
  if(sr) {
    while(structrecord_parent(sr) != NULL) {
      sr=structrecord_parent(sr);
    }
    structrecord_free(sr);
  }
  if(!l->map) {
    TS_LIST_CLEAR_AND_DELETE(l->tiles, tile);
  }
  if(l->tiles) { TCOD_list_delete(l->tiles); }
  free(l);
}

void maplistener_handle_events(MapListener listener, TCOD_list_t evts) {
  for(int i = 0; i < TCOD_list_size(evts); i++) {
    TCOD_parser_event_t *evt = TCOD_list_get(evts, i);
    switch(evt->type) {
      case TCOD_PARSER_EVENT_NEW_STRUCT:
        if(!maplistener_new_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          maplistener_error(listener, "bad struct start");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_FLAG:
        if(!maplistener_new_flag(listener, evt->event_flag.name)) {
          maplistener_error(listener, "bad flag");
        }
//        free(evt->event_flag.name);
        break;
      case TCOD_PARSER_EVENT_PROPERTY:
        if(!maplistener_new_property(listener, evt->event_property.name, evt->event_property.type, evt->event_property.value)) {
          maplistener_error(listener, "bad prop");
        }
//        free(evt->event_property.name);
        break;
      case TCOD_PARSER_EVENT_END_STRUCT:
        if(!maplistener_end_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          maplistener_error(listener, "bad struct end");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_ERROR:
        maplistener_error(listener, evt->event_error.message);
//        free(evt->event_error.message);
        break;
      default:
        maplistener_error(listener, "unrecognized parser event");
        break;
    }
//    free(evt);
  }
}

bool maplistener_new_struct(MapListener listener, TCOD_parser_struct_t str,const char *name) {
  listener->workingStruct = structrecord_init(
    structrecord_new(), 
    (char *)TCOD_struct_get_name(str), 
    (char *)name, 
    listener->workingStruct
  );
  return true;
}
bool maplistener_new_flag(MapListener listener, const char *name) {
  structrecord_add_flag(listener->workingStruct, (char *)name);
  return true;
}
bool maplistener_new_property(MapListener listener, const char *propname, TCOD_value_type_t type, TCOD_value_t value) {
  structrecord_add_prop(listener->workingStruct, prop_init(prop_new(), (char *)propname, type, value));
  return true;
}
/*
we need a listener class per file format.
The listener class determines how to interpret the top-level; for instance,
a MapListener will accumulate Tiles until its Map is ready, then it will add them.
If it sees any tiles after the map, it will add them too.
An ObjectListener will create archetypes, etc.
There needs to be some good way of sharing behavior between listeners so that not everybody
has to implement the same methods for e.g. movement flags and actions. probably a utility library
or something?  Some form of tree-based delegation to another listener for a while?

*/

bool maplistener_end_struct(MapListener l, TCOD_parser_struct_t str, const char *name) {
  StructRecord sr = l->workingStruct;
  //ending an intermediate struct, don't need to delete anything.
  if(structrecord_parent(sr) != NULL) {
    l->workingStruct = structrecord_parent(sr);
    return true;
  } else if(strcmp(TCOD_struct_get_name(str), "map") == 0) {
    if(l->map) { return false; }
    l->map = map_init_structrecord(map_new(), sr);
    //add any tiles that we're still sitting on
    for(int i = 0; i < TCOD_list_size(l->tiles); i++) {
      Tile t = TCOD_list_get(l->tiles, i);
      map_add_tile(l->map, t);
    }
    TCOD_list_clear(l->tiles);
    loader_add_map(l->loader, l->map, (char *)name);
    structrecord_free(sr);
    l->workingStruct = NULL;
    return true;
  } else if(strcmp(TCOD_struct_get_name(str), "tile") == 0) {
    //other stuff later! movement etc!
    Tile t = tile_init_structrecord(tile_new(), sr);
    if(l->map) {
      //add to map
      map_add_tile(l->map, t);
    } else {
      //add to list
      TCOD_list_push(l->tiles, t);
    }
    structrecord_free(sr);
    l->workingStruct = NULL;
    return true;
  }
  return false;
}
void maplistener_error(MapListener l, const char *msg) {
  abort(); //just completely bail out in case of any error.  can do something useful later!
}
