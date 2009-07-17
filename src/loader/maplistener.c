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

TCOD_parser_t maplistener_init_parser(TCOD_parser_t p, Loader l) {
  FlagSchema triggerSchema = loader_trigger_schema(l);
  TCOD_list_t moveFlags = loader_move_flags(l);
  
  TCOD_parser_struct_t grantst = TCOD_parser_new_struct(p, "grant");
  TCOD_struct_add_property(grantst, "duration", TCOD_TYPE_FLOAT, false); //defaults to infinity
  TCOD_struct_add_property(grantst, "priority", TCOD_TYPE_INT, false); //defaults to 1
  TCOD_struct_add_property(grantst, "reason", TCOD_TYPE_STRING, false); //defaults to NULL
  //this is actually only valid in grants {} on actions
  TCOD_struct_add_property(grantst, "target", TCOD_TYPE_STRING, false);

  TCOD_parser_struct_t revokest = TCOD_parser_new_struct(p, "revoke");
  TCOD_struct_add_property(revokest, "priority", TCOD_TYPE_INT, false); //defaults to 1
  TCOD_struct_add_property(revokest, "reason", TCOD_TYPE_STRING, false); //defaults to NULL
  //this is actually only valid in revokes {} on actions
  TCOD_struct_add_property(revokest, "target", TCOD_TYPE_STRING, false);

  TCOD_parser_struct_t checkst = TCOD_parser_new_struct(p, "check");
  TCOD_struct_add_property(checkst, "target", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "source", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "source_object", TCOD_TYPE_STRING, false);

  TCOD_struct_add_property(checkst, "greater_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "greater_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "less_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "less_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_flag(checkst, "greater_than");
  TCOD_struct_add_flag(checkst, "greater_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "less_than");
  TCOD_struct_add_flag(checkst, "less_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "equal_to");

  TCOD_struct_add_property(checkst, "starts_with", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "ends_with", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "find_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "is_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(checkst, "starts_with");
  TCOD_struct_add_flag(checkst, "ends_with");
  TCOD_struct_add_flag(checkst, "find_string");
  TCOD_struct_add_flag(checkst, "is_string");

  TCOD_struct_add_property(checkst, "count_greater_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_greater_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_less_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_less_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_flag(checkst, "count_greater_than");
  TCOD_struct_add_flag(checkst, "count_greater_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "count_less_than");
  TCOD_struct_add_flag(checkst, "count_less_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "count_equal_to");
  
  TCOD_struct_add_property(checkst, "contains", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(checkst, "contains_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(checkst, "contains_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(checkst, "contains_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(checkst, "contains");
  TCOD_struct_add_flag(checkst, "contains_string");
  TCOD_struct_add_flag(checkst, "contains_all");
  TCOD_struct_add_flag(checkst, "contains_all_strings");

  static const char *condition_modes[] = { "all", "any", NULL };    
  TCOD_parser_struct_t conditionst = TCOD_parser_new_struct(p, "condition");
  TCOD_struct_add_value_list(conditionst, "require", condition_modes, false); //defaults to 'all'
  TCOD_struct_add_flag(conditionst, "negate");
  TCOD_struct_add_structure(conditionst, checkst);
  TCOD_struct_add_structure(conditionst, conditionst);  

  TCOD_parser_struct_t setst = TCOD_parser_new_struct(p, "set");
  TCOD_struct_add_property(setst, "increase", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(setst, "decrease", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(setst, "set_number", TCOD_TYPE_FLOAT, false);
  //srcvar-using variants
  TCOD_struct_add_flag(setst, "increase");
  TCOD_struct_add_flag(setst, "decrease");
  TCOD_struct_add_flag(setst, "set_number");
  
  TCOD_struct_add_property(setst, "concat", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "excise", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "set_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(setst, "concat");
  TCOD_struct_add_flag(setst, "excise");
  TCOD_struct_add_flag(setst, "set_string");

  TCOD_struct_add_property(setst, "push", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "append_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(setst, "remove", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "remove_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "set_list", TCOD_TYPE_INT, false);  
  TCOD_struct_add_flag(setst, "push");
  TCOD_struct_add_flag(setst, "append_all");
  TCOD_struct_add_flag(setst, "remove");
  TCOD_struct_add_flag(setst, "remove_all");
  TCOD_struct_add_flag(setst, "set_list");

  TCOD_struct_add_property(setst, "push_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "append_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "remove_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "remove_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "set_string_list", TCOD_TYPE_STRING, false);  
  TCOD_struct_add_flag(setst, "push_string");
  TCOD_struct_add_flag(setst, "append_all_strings");
  TCOD_struct_add_flag(setst, "remove_string");
  TCOD_struct_add_flag(setst, "remove_all_strings");
  TCOD_struct_add_flag(setst, "set_string_list");
  

  TCOD_parser_struct_t actionst = TCOD_parser_new_struct(p, "action");
  TCOD_list_t triggers = flagschema_get_labels(triggerSchema);
  TS_LIST_FOREACH(triggers,
    TCOD_struct_add_flag(actionst, each);
  );
  TCOD_list_clear_and_delete(triggers);
  //timers not supported yet
  
  TCOD_struct_add_structure(actionst, conditionst);
  TCOD_struct_add_structure(actionst, grantst);  
  TCOD_struct_add_structure(actionst, revokest);  
  TCOD_struct_add_structure(actionst, setst);

  TCOD_parser_struct_t movst = TCOD_parser_new_struct(p, "movement");
  TS_LIST_FOREACH(moveFlags,
    TCOD_struct_add_property(movst, each, TCOD_TYPE_BOOL, false);
  );
  
  TCOD_parser_struct_t drawst = TCOD_parser_new_struct(p, "draw");
  TCOD_struct_add_property(drawst, "z", TCOD_TYPE_INT, false); //defaults to the index of the drawst.
  TCOD_struct_add_property(drawst, "fore", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "back", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "symbol", TCOD_TYPE_CHAR, true);  

  static const char *movement_defaults[] = { "allow", "deny", NULL };    

  TCOD_parser_struct_t tst = TCOD_parser_new_struct(p, "tile");
  //opacity
  TCOD_struct_add_list_property(tst, "opacity", TCOD_TYPE_CHAR, false); //defaults to [0,0,0,0,15,15,0,0] -- an opaque floor, no ceiling
  //opacity shorthands
  TCOD_struct_add_property(tst, "wall_opacity", TCOD_TYPE_CHAR, false); //defaults to 0
  TCOD_struct_add_property(tst, "floor_opacity", TCOD_TYPE_CHAR, false); //defaults to 15
  TCOD_struct_add_property(tst, "ceiling_opacity", TCOD_TYPE_CHAR, false); //defaults to 15
  TCOD_struct_add_property(tst, "uniform_opacity", TCOD_TYPE_CHAR, false); //defaults to 0

  //desc
  TCOD_struct_add_property(tst, "allow_desc", TCOD_TYPE_STRING, false); //defaults to ""
  TCOD_struct_add_property(tst, "deny_desc", TCOD_TYPE_STRING, false); //defaults to ""
  TCOD_struct_add_property(tst, "ontop_desc", TCOD_TYPE_STRING, false); //defaults to ""

  //stairs
  TCOD_struct_add_flag(tst, "stairs");
  TCOD_struct_add_property(tst, "up_desc", TCOD_TYPE_STRING, false); //defaults to ""
  TCOD_struct_add_property(tst, "down_desc", TCOD_TYPE_STRING, false); //defaults to ""

  //movement
  TCOD_struct_add_value_list(tst, "movement_default", movement_defaults, false); //defaults to "allow"
  TCOD_struct_add_structure(tst, movst);
  
  //actions
  TCOD_struct_add_structure(tst, actionst);  
  
  //drawing
  TCOD_struct_add_structure(tst, drawst);

  TCOD_parser_struct_t mapst = TCOD_parser_new_struct(p, "map");
  TCOD_struct_add_property(mapst, "ambient_light", TCOD_TYPE_CHAR, false); //defaults to 8
  TCOD_struct_add_list_property(mapst, "dimensions", TCOD_TYPE_INT, true); //no default
  TCOD_struct_add_list_property(mapst, "tilemap", TCOD_TYPE_CHAR, true); //no default
  
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
