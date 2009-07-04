#ifndef _LOADERLISTENER_H
#define _LOADERLISTENER_H

#include <libtcod.h>
#include <tilesense.h>

#include "loader.h"
#include "loader/prop.h"
#include "loader/structrecord.h"

struct _maplistener {
  Loader loader;
  StructRecord workingStruct;
  
  TCOD_list_t tiles;
  Map map;
};
typedef struct _maplistener * MapListener;

MapListener maplistener_new();
MapListener maplistener_init(MapListener l, Loader loader);
void maplistener_free(MapListener l);
TCOD_parser_listener_t *maplistener_listener(MapListener l);

bool maplistener_start_map(MapListener listener, char *name);
bool maplistener_finalize_map(MapListener listener);

void maplistener_handle_events(MapListener listener, TCOD_list_t evts);

bool maplistener_new_struct(MapListener l, TCOD_parser_struct_t str,const char *name);
bool maplistener_new_flag(MapListener l, const char *name);
bool maplistener_new_property(MapListener l, const char *propname, TCOD_value_type_t type, TCOD_value_t value);
bool maplistener_end_struct(MapListener l, TCOD_parser_struct_t str, const char *name);
void maplistener_error(MapListener l, const char *msg);
#endif