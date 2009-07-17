#ifndef _CONFIGLISTENER_H
#define _CONFIGLISTENER_H

#include <libtcod.h>
#include <tilesense.h>

#include "loader/prop.h"
#include "loader/structrecord.h"

#include "loader.h"

struct _configlistener {
  Loader loader;
  StructRecord workingStruct;
  TCOD_list_t statusFiles;
};
typedef struct _configlistener * ConfigListener;

TCOD_parser_t configlistener_init_parser(TCOD_parser_t p, Loader l);

ConfigListener configlistener_new();
ConfigListener configlistener_init(ConfigListener l, Loader loader);
void configlistener_free(ConfigListener l);

void configlistener_handle_events(ConfigListener listener, TCOD_list_t evts);

bool configlistener_new_struct(ConfigListener l, TCOD_parser_struct_t str,const char *name);
bool configlistener_new_flag(ConfigListener l, const char *name);
bool configlistener_new_property(ConfigListener l, const char *propname, TCOD_value_type_t type, TCOD_value_t value);
bool configlistener_end_struct(ConfigListener l, TCOD_parser_struct_t str, const char *name);
void configlistener_error(ConfigListener l, const char *msg);

TCOD_list_t configlistener_status_files(ConfigListener l);
#endif