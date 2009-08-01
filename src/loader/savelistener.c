#include "loader/savelistener.h"

#include <tilesense.h>

#include "loader/model_init_structrecord.h"

TCOD_parser_t savelistener_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t makest = TCOD_parser_new_struct(p, "make_object");
  TCOD_struct_add_property(makest, "map", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(makest, "id", TCOD_TYPE_STRING, true);
  TCOD_struct_add_list_property(makest, "position", TCOD_TYPE_INT, true);
  TCOD_struct_add_list_property(makest, "facing", TCOD_TYPE_FLOAT, false);
  //more object overrides later!
  
  TCOD_parser_struct_t mapst = TCOD_parser_new_struct(p, "map");
  TCOD_struct_add_structure(mapst, makest);
  
  TCOD_parser_struct_t savest = TCOD_parser_new_struct(p, "save");
  TCOD_struct_add_structure(savest, mapst);
  TCOD_struct_add_structure(savest, makest);

  return p;
}

SaveListener savelistener_new() {
  return calloc(1, sizeof(struct _savelistener));
}
SaveListener savelistener_init(SaveListener l, Loader loader) {
  l->loader = loader;
  l->workingStruct = NULL;
  return l;
}
void savelistener_free(SaveListener l) {
  StructRecord sr = l->workingStruct;
  if(sr) {
    while(structrecord_parent(sr) != NULL) {
      sr=structrecord_parent(sr);
    }
    structrecord_free(sr);
  }
  free(l);
}

void savelistener_handle_events(SaveListener listener, TCOD_list_t evts) {
  for(int i = 0; i < TCOD_list_size(evts); i++) {
    TCOD_parser_event_t *evt = TCOD_list_get(evts, i);
    switch(evt->type) {
      case TCOD_PARSER_EVENT_NEW_STRUCT:
        if(!savelistener_new_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          savelistener_error(listener, "bad struct start");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_FLAG:
        if(!savelistener_new_flag(listener, evt->event_flag.name)) {
          savelistener_error(listener, "bad flag");
        }
//        free(evt->event_flag.name);
        break;
      case TCOD_PARSER_EVENT_PROPERTY:
        if(!savelistener_new_property(listener, evt->event_property.name, evt->event_property.type, evt->event_property.value)) {
          savelistener_error(listener, "bad prop");
        }
//        free(evt->event_property.name);
        break;
      case TCOD_PARSER_EVENT_END_STRUCT:
        if(!savelistener_end_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          savelistener_error(listener, "bad struct end");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_ERROR:
        savelistener_error(listener, evt->event_error.message);
//        free(evt->event_error.message);
        break;
      default:
        savelistener_error(listener, "unrecognized parser event");
        break;
    }
//    free(evt);
  }
}

bool savelistener_new_struct(SaveListener listener, TCOD_parser_struct_t str,const char *name) {
  listener->workingStruct = structrecord_init(
    structrecord_new(), 
    (char *)TCOD_struct_get_name(str), 
    (char *)name, 
    listener->workingStruct
  );
  return true;
}
bool savelistener_new_flag(SaveListener listener, const char *name) {
  structrecord_add_flag(listener->workingStruct, (char *)name);
  return true;
}
bool savelistener_new_property(SaveListener listener, const char *propname, TCOD_value_type_t type, TCOD_value_t value) {
  structrecord_add_prop(listener->workingStruct, prop_init(prop_new(), (char *)propname, type, value));
  return true;
}

bool savelistener_end_struct(SaveListener l, TCOD_parser_struct_t str, const char *name) {
  StructRecord sr = l->workingStruct;
  //ending an intermediate struct, don't need to delete anything.
  if(structrecord_parent(sr) != NULL) {
    l->workingStruct = structrecord_parent(sr);
    return true;
  } else if(STREQ(structrecord_type(sr), "save")) {
    savelistener_load_save(l, sr);
    l->workingStruct = NULL;
    return true;
  }
  return false;
}
void savelistener_error(SaveListener l, const char *msg) {
  abort(); //just completely bail out in case of any error.  can do something useful later!
}

void savelistener_load_map(SaveListener l, StructRecord sr) {
  TCOD_list_t kids = structrecord_children(sr);
  TS_LIST_FOREACH(kids,
    char *t = structrecord_type(each);
    if(STREQ(t, "make_object")) {
      structrecord_add_prop(each, prop_init(prop_new(), "map", TCOD_TYPE_STRING, (TCOD_value_t)(structrecord_name(sr))));
      loader_make_object(l->loader, each);
    }
  );
}

void savelistener_load_save(SaveListener l, StructRecord sr) {
  TCOD_list_t kids = structrecord_children(sr);
  #warning weird to put the map-force-load here. somewhere else maybe?
  TS_LIST_FOREACH(kids,
    char *t = structrecord_type(each);
    if(STREQ(t, "map")) {
      loader_load_map(l->loader, structrecord_name(each));
      savelistener_load_map(l, each);
    } else if(STREQ(t, "make_object")) {
      loader_make_object(l->loader, each);
    }
  );
}
