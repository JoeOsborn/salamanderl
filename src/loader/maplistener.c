#include "loader/maplistener.h"

#include "drawinfo.h"
#include "moveinfo.h"
#include "tileinfo.h"

#include "action/action.h"
#include "action/bindings.h"
#include "action/check.h"
#include "action/comparison.h"
#include "action/condition.h"
#include "action/effect_grantrevoke.h"
#include "action/effect_set.h"

#include <tilesense.h>

#include "loader/model_init_structrecord.h"
#include "loader/model_init_parser.h"


TCOD_parser_t maplistener_init_parser(TCOD_parser_t p, Loader l) {
  tile_init_parser(p, l);
  map_init_parser(p, l);  
  return p;
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
    Tile t = tile_init_structrecord(tile_new(), l->loader, sr, loader_trigger_schema(l->loader));
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
