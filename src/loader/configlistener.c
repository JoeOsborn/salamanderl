#include "loader/configlistener.h"

#include "drawinfo.h"
#include "moveinfo.h"
#include "tileinfo.h"

#include <tilesense.h>

#include "loader/model_init_structrecord.h"

TCOD_parser_t configlistener_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t cfgst = TCOD_parser_new_struct(p, "config");
  TCOD_struct_add_list_property(cfgst, "trigger_types", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(cfgst, "load_status", TCOD_TYPE_STRING, false);
  return p;
}

ConfigListener configlistener_new() {
  return calloc(1, sizeof(struct _configlistener));
}
ConfigListener configlistener_init(ConfigListener l, Loader loader) {
  l->loader = loader;
  l->workingStruct = NULL;
  return l;
}
void configlistener_free(ConfigListener l) {
  StructRecord sr = l->workingStruct;
  if(sr) {
    while(structrecord_parent(sr) != NULL) {
      sr=structrecord_parent(sr);
    }
    structrecord_free(sr);
  }
  free(l);
}

void configlistener_handle_events(ConfigListener listener, TCOD_list_t evts) {
  for(int i = 0; i < TCOD_list_size(evts); i++) {
    TCOD_parser_event_t *evt = TCOD_list_get(evts, i);
    switch(evt->type) {
      case TCOD_PARSER_EVENT_NEW_STRUCT:
        if(!configlistener_new_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          configlistener_error(listener, "bad struct start");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_FLAG:
        if(!configlistener_new_flag(listener, evt->event_flag.name)) {
          configlistener_error(listener, "bad flag");
        }
//        free(evt->event_flag.name);
        break;
      case TCOD_PARSER_EVENT_PROPERTY:
        if(!configlistener_new_property(listener, evt->event_property.name, evt->event_property.type, evt->event_property.value)) {
          configlistener_error(listener, "bad prop");
        }
//        free(evt->event_property.name);
        break;
      case TCOD_PARSER_EVENT_END_STRUCT:
        if(!configlistener_end_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          configlistener_error(listener, "bad struct end");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_ERROR:
        configlistener_error(listener, evt->event_error.message);
//        free(evt->event_error.message);
        break;
      default:
        configlistener_error(listener, "unrecognized parser event");
        break;
    }
//    free(evt);
  }
}

bool configlistener_new_struct(ConfigListener listener, TCOD_parser_struct_t str,const char *name) {
  listener->workingStruct = structrecord_init(
    structrecord_new(), 
    (char *)TCOD_struct_get_name(str), 
    (char *)name, 
    listener->workingStruct
  );
  return true;
}
bool configlistener_new_flag(ConfigListener listener, const char *name) {
  structrecord_add_flag(listener->workingStruct, (char *)name);
  return true;
}
bool configlistener_new_property(ConfigListener listener, const char *propname, TCOD_value_type_t type, TCOD_value_t value) {
  structrecord_add_prop(listener->workingStruct, prop_init(prop_new(), (char *)propname, type, value));
  return true;
}

bool configlistener_end_struct(ConfigListener l, TCOD_parser_struct_t str, const char *name) {
  StructRecord sr = l->workingStruct;
  //ending an intermediate struct, don't need to delete anything.
  if(structrecord_parent(sr) != NULL) {
    l->workingStruct = structrecord_parent(sr);
    return true;
  } else if(strcmp(TCOD_struct_get_name(str), "config") == 0) {
    //does it have a trigger_types?
    if(structrecord_has_prop(sr, "trigger_types")) {
      TCOD_list_t trigs = structrecord_get_prop_value(sr, "trigger_types").list;
      TS_LIST_FOREACH(trigs,
        loader_add_trigger(l->loader, each);
      );
    }
    //does it have a load_status?
    if(structrecord_has_prop(sr, "load_status")) {
      TCOD_list_t statusFiles = structrecord_get_prop_value(sr, "load_status").list;
      TS_LIST_FOREACH(statusFiles,
        loader_load_status(l->loader, each);
      );
    }
    structrecord_free(sr);
    l->workingStruct = NULL;
    return true;
  }
  return false;
}
void configlistener_error(ConfigListener l, const char *msg) {
  abort(); //just completely bail out in case of any error.  can do something useful later!
}
