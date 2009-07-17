#ifndef _OBJECTLISTENER_H
#define _OBJECTLISTENER_H

#include <libtcod.h>
#include <tilesense.h>

#include "loader/prop.h"
#include "loader/structrecord.h"

#include "loader.h"

struct _objectlistener {
  Loader loader;
  StructRecord workingStruct;
};
typedef struct _objectlistener * ObjectListener;

TCOD_parser_t objectlistener_init_parser(TCOD_parser_t p, Loader l);

ObjectListener objectlistener_new();
ObjectListener objectlistener_init(ObjectListener l, Loader loader);
void objectlistener_free(ObjectListener l);

void objectlistener_handle_events(ObjectListener listener, TCOD_list_t evts);

bool objectlistener_new_struct(ObjectListener l, TCOD_parser_struct_t str,const char *name);
bool objectlistener_new_flag(ObjectListener l, const char *name);
bool objectlistener_new_property(ObjectListener l, const char *propname, TCOD_value_type_t type, TCOD_value_t value);
bool objectlistener_end_struct(ObjectListener l, TCOD_parser_struct_t str, const char *name);
void objectlistener_error(ObjectListener l, const char *msg);
#endif