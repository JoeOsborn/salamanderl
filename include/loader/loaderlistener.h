#ifndef _LOADERLISTENER_H
#define _LOADERLISTENER_H

#include <libtcod.h>

#include "loader.h"
#include "loader/prop.h"
#include "loader/structrecord.h"

struct _loaderlistener {
  Loader loader;
  TCOD_parser_listener_t listener;
  StructRecord workingRecord;
  TCOD_list_t oldStructs;
};
typedef struct _loaderlistener * LoaderListener;

LoaderListener loader_listener_new();
LoaderListener loader_listener_init(LoaderListener l, Loader loader);
void loader_listener_free(LoaderListener l);
TCOD_parser_listener_t loader_listener_listener(LoaderListener l);

bool loader_listener_start_map(LoaderListener listener, char *name);
bool loader_listener_finalize_map(LoaderListener listener);

bool loader_listener_new_struct(TCOD_parser_struct_t str,const char *name, void *ctx);
bool loader_listener_new_flag(const char *name, void *ctx);
bool loader_listener_new_property(const char *propname, TCOD_value_type_t type, TCOD_value_t value, void *ctx);
bool loader_listener_end_struct(TCOD_parser_struct_t str, const char *name, void *ctx);
void loader_listener_error(const char *msg, void *ctx);

#endif