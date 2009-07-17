#ifndef _MAPLISTENER_H
#define _MAPLISTENER_H

#include <libtcod.h>
#include <tilesense.h>

#include "loader/prop.h"
#include "loader/structrecord.h"

#include "loader.h"

struct _maplistener {
  Loader loader;
  StructRecord workingStruct;
  
  FlagSchema triggerSchema;
  
  TCOD_list_t tiles;
  Map map;
};
typedef struct _maplistener * MapListener;

TCOD_parser_t maplistener_init_parser(TCOD_parser_t p, Loader l);

MapListener maplistener_new();
MapListener maplistener_init(MapListener l, Loader loader);
void maplistener_free(MapListener l);

void maplistener_handle_events(MapListener listener, TCOD_list_t evts);

bool maplistener_new_struct(MapListener l, TCOD_parser_struct_t str,const char *name);
bool maplistener_new_flag(MapListener l, const char *name);
bool maplistener_new_property(MapListener l, const char *propname, TCOD_value_type_t type, TCOD_value_t value);
bool maplistener_end_struct(MapListener l, TCOD_parser_struct_t str, const char *name);
void maplistener_error(MapListener l, const char *msg);
#endif