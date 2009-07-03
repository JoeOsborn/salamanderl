#include "loader/loaderlistener.h"

MapListener maplistener_new() {
  return calloc(1, sizeof(struct _maplistener));
}
MapListener maplistener_init(MapListener l, Loader loader) {
  l->loader = loader;
  l->listener = (TCOD_parser_listener_t) {
  	maplistener_new_struct,
  	maplistener_new_flag,
  	maplistener_new_property,
  	maplistener_end_struct,
  	maplistener_error,
  	l
  };
  l->workingStruct = NULL;
  l->tiles = TCOD_list_new();
  return l;
}
void maplistener_free(MapListener l) {
  if(l->workingStruct) {
    structrecord_free(l->workingStruct);
  }
  free(l);
}
TCOD_parser_listener_t maplistener_listener(MapListener l) {
  return l->listener;
}

bool maplistener_new_struct(TCOD_parser_struct_t str,const char *name, void *ctx) {
  MapListener listener = (MapListener)(ctx);
  listener->workingStruct = structrecord_init(
    structrecord_new(), 
    TCOD_struct_get_name(str), 
    name, 
    listener->workingStruct
  );
  return true;
}
bool maplistener_new_flag(const char *name, void *ctx) {
  MapListener listener = (MapListener)(ctx);
  structrecord_add_flag(workingStruct, name);
  return true;
}
bool maplistener_new_property(const char *propname, TCOD_value_type_t type, TCOD_value_t value, void *ctx) {
  MapListener listener = (MapListener)(ctx);
  structrecord_add_property(workingStruct, prop_init(prop_new(), propname, type, value));
  return true;
}
#warning no time to finish this now, but probably this is how things should go down:
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
bool maplistener_end_struct(TCOD_parser_struct_t str, const char *name, void *ctx) {
  MapListener listener = (MapListener)(ctx);
  StructRecord sr = l->workingRecord;
  if(strcmp(TCOD_struct_get_name(str), "map") == 0) {
    if(l->map) { return false; }
    //dimensions, ambient light, tilemap
    //implement these search methods
    int ambientLight = structrecord_get_prop_value(sr, "ambient_light").i;
    TCOD_list_t dimensions = structrecord_get_prop_value(sr, "dimensions").list;
    //make mapSize from floats in dimensions
    //make tileMap from the TCOD_list_t in "tilemap"
    unsigned short *tileMap = NULL;
    //make a mapCtx that stores other info.  guess it can be null for now
    Map m = map_init(map_new(), name, mapSize, tileMap, ambientLight, NULL, mapCtx);
    //add any tiles that we're still sitting on
    for(int i = 0; i < TCOD_list_size(listener->tiles); i++) {
      Tile t = TCOD_list_get(listener->tiles, i);
      if(strcmp(structrecord_type(sr), "tile") == 0) {
        map_add_tile(t);
        //consumed it, no longer needed
        TCOD_list_remove(listener->tiles, t);
      }
    }
    loader_add_map(listener->loader, m, name);
  } else if(strcmp(TCOD_struct_get_name(str), "tile") == 0) {
    //opacity, fore, back, symbol
    //, ...      
    Tile t = tile_init(tile_new(), );
    if(l->map) {
      //add to map
    } else {
      //add to list
      TCOD_list_push(listener->tiles, t);
    }
  }
  return false;
}
void maplistener_error(const char *msg, void *ctx) {
  exit(-1); //just completely bail out in case of any error.  can do something useful later!
}
