#include "loader/loaderlistener.h"

LoaderListener loader_listener_new() {
  return calloc(1, sizeof(struct _loaderlistener));
}
LoaderListener loader_listener_init(LoaderListener l, Loader loader) {
  l->loader = loader;
  l->listener = (TCOD_parser_listener_t) {
  	loader_listener_new_struct,
  	loader_listener_new_flag,
  	loader_listener_new_property,
  	loader_listener_end_struct,
  	loader_listener_error,
  	l
  };
  l->workingRecord = NULL;
  return l;
}
void loader_listener_free(LoaderListener l) {
  if(l->workingRecord) {
    structrecord_free(l->workingRecord);
  }
  free(l);
}
TCOD_parser_listener_t loader_listener_listener(LoaderListener l) {
  return l->listener;
}

bool loader_listener_new_struct(TCOD_parser_struct_t str,const char *name, void *ctx) {
  LoaderListener listener = (LoaderListener)(ctx);
  listener->workingRecord = structrecord_init(
    structrecord_new(), 
    TCOD_struct_get_name(str), 
    name, 
    listener->workingRecord
  );

  return false;
}
bool loader_listener_new_flag(const char *name, void *ctx) {
  LoaderListener listener = (LoaderListener)(ctx);
  return false;
}
bool loader_listener_new_property(const char *propname, TCOD_value_type_t type, TCOD_value_t value, void *ctx) {
  LoaderListener listener = (LoaderListener)(ctx);
  return false;
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
bool loader_listener_end_struct(TCOD_parser_struct_t str, const char *name, void *ctx) {
  LoaderListener listener = (LoaderListener)(ctx);
  if(strcmp(TCOD_struct_get_name(str), "map") == 0) {
    Map m = NULL;
    //add any tiles that we're still sitting on
    for(int i = 0; i < TCOD_list_size(listener->oldStructs); i++) {
      StructRecord sr = TCOD_list_get(listener->oldStructs, i);
      if(strcmp(structrecord_type(sr), "tile") == 0) {
        map_add_tile(blah);
        //consumed it, no longer needed?
        TCOD_list_remove(sr);
        structrecord_free(sr);
      }
    }
    loader_add_map(listener->loader, m, name);
  } else if()
  return false;
}
void loader_listener_error(const char *msg, void *ctx) {
  exit(-1); //just completely bail out in case of any error.  can do something useful later!
}
