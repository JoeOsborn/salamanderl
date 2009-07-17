#include "loader/objectlistener.h"

#include <tilesense.h>

#include "loader/model_init_structrecord.h"

TCOD_parser_t objectlistener_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_list_t moveFlags = loader_move_flags(l);

  //factor out these first two somehow
  
  TCOD_parser_struct_t movest = TCOD_parser_new_struct(p, "movement");
  TS_LIST_FOREACH(moveFlags,
    TCOD_struct_add_property(movest, each, TCOD_TYPE_BOOL, false);
  );
  
  TCOD_parser_struct_t drawst = TCOD_parser_new_struct(p, "draw");
  TCOD_struct_add_property(drawst, "z", TCOD_TYPE_INT, false); //defaults to the index of the drawst.
  TCOD_struct_add_property(drawst, "fore", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "back", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "symbol", TCOD_TYPE_CHAR, true);  
  
  TCOD_parser_struct_t spherest = TCOD_parser_new_struct(p, "sphere");
  TCOD_struct_add_list_property(spherest, "position", TCOD_TYPE_FLOAT, false); //defaults to [0,0,0]
  TCOD_struct_add_property(spherest, "radius", TCOD_TYPE_FLOAT, false); //defaults to 5
  
  TCOD_parser_struct_t frustumst = TCOD_parser_new_struct(p, "frustum");
  TCOD_struct_add_list_property(frustumst, "position", TCOD_TYPE_FLOAT, false); //defaults to [0,0,0]
  TCOD_struct_add_list_property(frustumst, "facing", TCOD_TYPE_FLOAT, false); //defaults to [1,0,0]
  TCOD_struct_add_property(frustumst, "xfov", TCOD_TYPE_FLOAT, false); //defaults to 2
  TCOD_struct_add_property(frustumst, "zfov", TCOD_TYPE_FLOAT, false); //defaults to 2
  TCOD_struct_add_property(frustumst, "near", TCOD_TYPE_FLOAT, false); //defaults to 0
  TCOD_struct_add_property(frustumst, "far", TCOD_TYPE_FLOAT, false); //defaults to 5

  TCOD_parser_struct_t boxst = TCOD_parser_new_struct(p, "box");
  TCOD_struct_add_list_property(boxst, "position", TCOD_TYPE_FLOAT, false); //defaults to [0,0,0]
  TCOD_struct_add_list_property(boxst, "facing", TCOD_TYPE_FLOAT, false); //defaults to [1,0,0]
  TCOD_struct_add_list_property(boxst, "extent", TCOD_TYPE_FLOAT, false); //defaults to [5,5,5]

  TCOD_parser_struct_t aaboxst = TCOD_parser_new_struct(p, "aabox");
  TCOD_struct_add_list_property(aaboxst, "position", TCOD_TYPE_FLOAT, false); //defaults to [0,0,0]
  TCOD_struct_add_list_property(aaboxst, "extent", TCOD_TYPE_FLOAT, false); //defaults to [5,5,5]
  
  TCOD_parser_struct_t sensorst = TCOD_parser_new_struct(p, "sensor");
  TCOD_struct_add_structure(sensorst, spherest);
  TCOD_struct_add_structure(sensorst, frustumst);
  TCOD_struct_add_structure(sensorst, boxst);
  TCOD_struct_add_structure(sensorst, aaboxst);
  
  TCOD_parser_struct_t objectst = TCOD_parser_new_struct(p, "object");
  TCOD_struct_add_structure(objectst, sensorst);
  TCOD_struct_add_structure(objectst, movest);
  TCOD_struct_add_structure(objectst, drawst);
  static const char *chompTypes[] = {"no", "eat", "carry", "latch", NULL};
  TCOD_struct_add_value_list(objectst, "chomp", chompTypes, false);
  
  TCOD_struct_add_property(objectst, "food_volume", TCOD_TYPE_FLOAT, false); //defaults to 0.5
  TCOD_struct_add_property(objectst, "food_digest_time", TCOD_TYPE_INT, false); //defaults to 60 seconds
  TCOD_struct_add_property(objectst, "weight", TCOD_TYPE_INT, false); //defaults to 125 grams, apprx the average weight of a salamander

  TCOD_struct_add_property(objectst, "description", TCOD_TYPE_STRING, false); //defaults to "".
  //actions?
  
    /*
    object
    	movement{}
    	draw{}
    	sensor{}
        ...
    		profile {} ? //for things like infrared vision, vision through walls, etc
    	action{} ?

    	chomp="eat"/"carry"/"latch"/"no"
    	food_volume=0.0-1.0 //fraction of full stomach - checked by check "fullness" {} clauses
    	food_digest_time=seconds
    	weight=grams //implies how much one eating or carrying this slows down, and net carry weight can be checked by check "weight" {} clauses.
//used for both food and carried objects!  can be separately defined as eat_weight and carry_weight?  different buttons for carry and eat??

    	//can objects block movement? if so, they need blocked moveinfos and stuff here
    	//can objects block light? if so, they need opacity data.
    */
  
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
