#ifndef _LOADERLISTENER_H
#define _LOADERLISTENER_H

#include <libtcod.h>

#include "loader.h"
#include "loader/prop.h"
#include "loader/structrecord.h"

struct _maplistener {
  Loader loader;
  TCOD_parser_listener_t listener;
  StructRecord workingStruct;
  
  TCOD_list_t tiles;
  Map map;
};
typedef struct _maplistener * MapListener;

MapListener maplistener_new();
MapListener maplistener_init(MapListener l, Loader loader);
void maplistener_free(MapListener l);
TCOD_parser_listener_t maplistener_listener(MapListener l);

bool maplistener_start_map(MapListener listener, char *name);
bool maplistener_finalize_map(MapListener listener);

bool maplistener_new_struct(TCOD_parser_struct_t str,const char *name, void *ctx);
bool maplistener_new_flag(const char *name, void *ctx);
bool maplistener_new_property(const char *propname, TCOD_value_type_t type, TCOD_value_t value, void *ctx);
bool maplistener_end_struct(TCOD_parser_struct_t str, const char *name, void *ctx);
void maplistener_error(const char *msg, void *ctx);

#endif