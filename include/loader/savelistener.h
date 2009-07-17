#ifndef _SAVELISTENER_H
#define _SAVELISTENER_H

#include <libtcod.h>
#include <tilesense.h>

#include "loader/prop.h"
#include "loader/structrecord.h"

#include "loader.h"

struct _savelistener {
  Loader loader;
  StructRecord workingStruct;
};
typedef struct _savelistener * SaveListener;

TCOD_parser_t savelistener_init_parser(TCOD_parser_t p, Loader l);

SaveListener savelistener_new();
SaveListener savelistener_init(SaveListener l, Loader loader);
void savelistener_free(SaveListener l);

void savelistener_handle_events(SaveListener listener, TCOD_list_t evts);

bool savelistener_new_struct(SaveListener l, TCOD_parser_struct_t str,const char *name);
bool savelistener_new_flag(SaveListener l, const char *name);
bool savelistener_new_property(SaveListener l, const char *propname, TCOD_value_type_t type, TCOD_value_t value);
bool savelistener_end_struct(SaveListener l, TCOD_parser_struct_t str, const char *name);
void savelistener_error(SaveListener l, const char *msg);

void savelistener_load_map(SaveListener l, StructRecord sr);
void savelistener_load_save(SaveListener l, StructRecord sr);
#endif