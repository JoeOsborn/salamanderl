#include "loader/loaderlistener.h"

#include "drawinfo.h"

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

Tile tile_init_structrecord(Tile t, StructRecord sr) {
  TCOD_list_t drawInfos = TCOD_list_new();
  int drawIndex = 0;
  for(int i = 0; i < TCOD_list_size(structrecord_children(sr)); i++) {
    StructRecord kid = TCOD_list_get(structrecord_children(sr), i);
    if(strcmp(structrecord_type(kid), "draw") == 0) {
      TCOD_list_push(drawInfos, drawinfo_init_structrecord(drawinfo_new(), kid, drawIndex, &drawIndex));
      drawIndex++;
    }
  }
  Flagset opacity = tile_opacity_flagset_make();
  //use base first
  if(structrecord_has_prop(sr, "opacity")) {
    TCOD_list_t opacityFields = structrecord_get_prop_value(sr, "opacity").list;
    tile_opacity_flagset_set(opacity,
      (unsigned int)TCOD_list_get(opacityFields, 0),
      (unsigned int)TCOD_list_get(opacityFields, 1),
      (unsigned int)TCOD_list_get(opacityFields, 2),
      (unsigned int)TCOD_list_get(opacityFields, 3),
      (unsigned int)TCOD_list_get(opacityFields, 4),
      (unsigned int)TCOD_list_get(opacityFields, 5)
    );
  }
  //then use any shorthands
  if(structrecord_has_prop(sr, "uniform_opacity")) {
    char uniformOpacity = structrecord_get_prop_value(sr, "uniform_opacity").c;
    tile_opacity_flagset_set(opacity,
      uniformOpacity,
      uniformOpacity,
      uniformOpacity,
      uniformOpacity,
      uniformOpacity,
      uniformOpacity
    );
  }
  if(structrecord_has_prop(sr, "wall_opacity")) {
    char wallOpacity = structrecord_get_prop_value(sr, "wall_opacity").c;
    tile_opacity_flagset_set(opacity,
      wallOpacity,
      wallOpacity,
      wallOpacity,
      wallOpacity,
      -1,
      -1
    );
  }
  if(structrecord_has_prop(sr, "floor_opacity")) {
    char floorOpacity = structrecord_get_prop_value(sr, "floor_opacity").c;
    tile_opacity_flagset_set(opacity,
      -1,
      -1,
      -1,
      -1,
      floorOpacity,
      floorOpacity
    );
  }
  #warning descs are being ignored
  return tile_init(t, opacity, drawInfos);
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
    for(int i = 0; i < TCOD_list_size(l->tiles); i++) {
      tile_free(TCOD_list_get(l->tiles, i));
    }
  }
  TCOD_list_delete(l->tiles);
  free(l);
}

void maplistener_handle_events(MapListener listener, TCOD_list_t evts) {
  for(int i = 0; i < TCOD_list_size(evts); i++) {
    TCOD_parser_event_t *evt = TCOD_list_get(evts, i);
    switch(evt->type) {
      case TCOD_PARSER_EVENT_NEW_STRUCT:
        if(!maplistener_new_struct(listener, evt->event.event_struct.str, evt->event.event_struct.name)) {
          maplistener_error(listener, "bad struct start");
        }
        if(evt->event.event_struct.name) {
//          free(evt->event.event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_FLAG:
        if(!maplistener_new_flag(listener, evt->event.event_flag.name)) {
          maplistener_error(listener, "bad flag");
        }
//        free(evt->event.event_flag.name);
        break;
      case TCOD_PARSER_EVENT_PROPERTY:
        if(!maplistener_new_property(listener, evt->event.event_property.name, evt->event.event_property.type, evt->event.event_property.value)) {
          maplistener_error(listener, "bad prop");
        }
//        free(evt->event.event_property.name);
        break;
      case TCOD_PARSER_EVENT_END_STRUCT:
        if(!maplistener_end_struct(listener, evt->event.event_struct.str, evt->event.event_struct.name)) {
          maplistener_error(listener, "bad struct end");
        }
        if(evt->event.event_struct.name) {
//          free(evt->event.event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_ERROR:
        maplistener_error(listener, evt->event.event_error.message);
//        free(evt->event.event_error.message);
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
