#include "loader/statuslistener.h"

#include <tilesense.h>

#include "loader/model_init_structrecord.h"

TCOD_parser_t statuslistener_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_list_t moveFlags = loader_move_flags(l);

  //factor out these first three somehow
  
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
  
  TCOD_parser_struct_t movest = TCOD_parser_new_struct(p, "movement");
  TS_LIST_FOREACH(moveFlags,
    TCOD_struct_add_property(movest, each, TCOD_TYPE_BOOL, false);
  );
  
  TCOD_parser_struct_t onst = TCOD_parser_new_struct(p, "on");
  TCOD_struct_add_structure(onst, grantst);
  TCOD_struct_add_structure(onst, revokest);
  TCOD_struct_add_structure(onst, movest);
  TCOD_struct_add_property(onst, "description", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(onst, "message", TCOD_TYPE_STRING, false);

  TCOD_parser_struct_t offst = TCOD_parser_new_struct(p, "off");
  TCOD_struct_add_structure(offst, grantst);
  TCOD_struct_add_structure(offst, revokest);
  TCOD_struct_add_property(offst, "message", TCOD_TYPE_STRING, false);
  
  TCOD_parser_struct_t statst = TCOD_parser_new_struct(p, "status");

  TCOD_struct_add_structure(statst, onst);
  TCOD_struct_add_structure(statst, offst);
  
  return p;
}

StatusListener statuslistener_new() {
  return calloc(1, sizeof(struct _statuslistener));
}
StatusListener statuslistener_init(StatusListener l, Loader loader) {
  l->loader = loader;
  l->workingStruct = NULL;
  return l;
}
void statuslistener_free(StatusListener l) {
  StructRecord sr = l->workingStruct;
  if(sr) {
    while(structrecord_parent(sr) != NULL) {
      sr=structrecord_parent(sr);
    }
    structrecord_free(sr);
  }
  free(l);
}

void statuslistener_handle_events(StatusListener listener, TCOD_list_t evts) {
  for(int i = 0; i < TCOD_list_size(evts); i++) {
    TCOD_parser_event_t *evt = TCOD_list_get(evts, i);
    switch(evt->type) {
      case TCOD_PARSER_EVENT_NEW_STRUCT:
        if(!statuslistener_new_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          statuslistener_error(listener, "bad struct start");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_FLAG:
        if(!statuslistener_new_flag(listener, evt->event_flag.name)) {
          statuslistener_error(listener, "bad flag");
        }
//        free(evt->event_flag.name);
        break;
      case TCOD_PARSER_EVENT_PROPERTY:
        if(!statuslistener_new_property(listener, evt->event_property.name, evt->event_property.type, evt->event_property.value)) {
          statuslistener_error(listener, "bad prop");
        }
//        free(evt->event_property.name);
        break;
      case TCOD_PARSER_EVENT_END_STRUCT:
        if(!statuslistener_end_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          statuslistener_error(listener, "bad struct end");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_ERROR:
        statuslistener_error(listener, evt->event_error.message);
//        free(evt->event_error.message);
        break;
      default:
        statuslistener_error(listener, "unrecognized parser event");
        break;
    }
//    free(evt);
  }
}

bool statuslistener_new_struct(StatusListener listener, TCOD_parser_struct_t str,const char *name) {
  listener->workingStruct = structrecord_init(
    structrecord_new(), 
    (char *)TCOD_struct_get_name(str), 
    (char *)name, 
    listener->workingStruct
  );
  return true;
}
bool statuslistener_new_flag(StatusListener listener, const char *name) {
  structrecord_add_flag(listener->workingStruct, (char *)name);
  return true;
}
bool statuslistener_new_property(StatusListener listener, const char *propname, TCOD_value_type_t type, TCOD_value_t value) {
  structrecord_add_prop(listener->workingStruct, prop_init(prop_new(), (char *)propname, type, value));
  return true;
}

bool statuslistener_end_struct(StatusListener l, TCOD_parser_struct_t str, const char *name) {
  StructRecord sr = l->workingStruct;
  //ending an intermediate struct, don't need to delete anything.
  if(structrecord_parent(sr) != NULL) {
    l->workingStruct = structrecord_parent(sr);
    return true;
  } else if(STREQ(structrecord_type(sr), "status")) {
    Status s = status_init_structrecord(status_new(), sr);
    loader_add_status(l->loader, s, structrecord_name(sr));
    structrecord_free(sr);
    l->workingStruct = NULL;
    return true;
  }
  return false;
}
void statuslistener_error(StatusListener l, const char *msg) {
  abort(); //just completely bail out in case of any error.  can do something useful later!
}
