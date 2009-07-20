#include "loader/objectlistener.h"

#include <tilesense.h>

#include "loader/model_init_structrecord.h"
#include "loader/model_init_parser.h"

TCOD_parser_t objectlistener_init_parser(TCOD_parser_t p, Loader l) {  
  object_init_parser(p, l);
  //actions?
  return p;
}

ObjectListener objectlistener_new() {
  return calloc(1, sizeof(struct _objectlistener));
}
ObjectListener objectlistener_init(ObjectListener l, Loader loader) {
  l->loader = loader;
  l->workingStruct = NULL;
  return l;
}
void objectlistener_free(ObjectListener l) {
  StructRecord sr = l->workingStruct;
  if(sr) {
    while(structrecord_parent(sr) != NULL) {
      sr=structrecord_parent(sr);
    }
    structrecord_free(sr);
  }
  free(l);
}

void objectlistener_handle_events(ObjectListener listener, TCOD_list_t evts) {
  for(int i = 0; i < TCOD_list_size(evts); i++) {
    TCOD_parser_event_t *evt = TCOD_list_get(evts, i);
    switch(evt->type) {
      case TCOD_PARSER_EVENT_NEW_STRUCT:
        if(!objectlistener_new_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          objectlistener_error(listener, "bad struct start");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_FLAG:
        if(!objectlistener_new_flag(listener, evt->event_flag.name)) {
          objectlistener_error(listener, "bad flag");
        }
//        free(evt->event_flag.name);
        break;
      case TCOD_PARSER_EVENT_PROPERTY:
        if(!objectlistener_new_property(listener, evt->event_property.name, evt->event_property.type, evt->event_property.value)) {
          objectlistener_error(listener, "bad prop");
        }
//        free(evt->event_property.name);
        break;
      case TCOD_PARSER_EVENT_END_STRUCT:
        if(!objectlistener_end_struct(listener, evt->event_struct.str, evt->event_struct.name)) {
          objectlistener_error(listener, "bad struct end");
        }
        if(evt->event_struct.name) {
//          free(evt->event_struct.name);
        }
        break;
      case TCOD_PARSER_EVENT_ERROR:
        objectlistener_error(listener, evt->event_error.message);
//        free(evt->event_error.message);
        break;
      default:
        objectlistener_error(listener, "unrecognized parser event");
        break;
    }
//    free(evt);
  }
}

bool objectlistener_new_struct(ObjectListener listener, TCOD_parser_struct_t str,const char *name) {
  listener->workingStruct = structrecord_init(
    structrecord_new(), 
    (char *)TCOD_struct_get_name(str), 
    (char *)name, 
    listener->workingStruct
  );
  return true;
}
bool objectlistener_new_flag(ObjectListener listener, const char *name) {
  structrecord_add_flag(listener->workingStruct, (char *)name);
  return true;
}
bool objectlistener_new_property(ObjectListener listener, const char *propname, TCOD_value_type_t type, TCOD_value_t value) {
  structrecord_add_prop(listener->workingStruct, prop_init(prop_new(), (char *)propname, type, value));
  return true;
}

bool objectlistener_end_struct(ObjectListener l, TCOD_parser_struct_t str, const char *name) {
  StructRecord sr = l->workingStruct;
  //ending an intermediate struct, don't need to delete anything.
  if(structrecord_parent(sr) != NULL) {
    l->workingStruct = structrecord_parent(sr);
    return true;
  } else if(STREQ(structrecord_type(sr), "object")) {
    loader_add_object_def(l->loader, sr);
    l->workingStruct = NULL;
    return true;
  }
  return false;
}
void objectlistener_error(ObjectListener l, const char *msg) {
  abort(); //just completely bail out in case of any error.  can do something useful later!
}
