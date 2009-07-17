#ifndef _STATUSLISTENER_H
#define _STATUSLISTENER_H

#include <libtcod.h>
#include <tilesense.h>

#include "loader/prop.h"
#include "loader/structrecord.h"

#include "loader.h"

struct _statuslistener {
  Loader loader;
  StructRecord workingStruct;
};
typedef struct _statuslistener * StatusListener;

TCOD_parser_t statuslistener_init_parser(TCOD_parser_t p, Loader l);

StatusListener statuslistener_new();
StatusListener statuslistener_init(StatusListener l, Loader loader);
void statuslistener_free(StatusListener l);

void statuslistener_handle_events(StatusListener listener, TCOD_list_t evts);

bool statuslistener_new_struct(StatusListener l, TCOD_parser_struct_t str,const char *name);
bool statuslistener_new_flag(StatusListener l, const char *name);
bool statuslistener_new_property(StatusListener l, const char *propname, TCOD_value_type_t type, TCOD_value_t value);
bool statuslistener_end_struct(StatusListener l, TCOD_parser_struct_t str, const char *name);
void statuslistener_error(StatusListener l, const char *msg);
#endif