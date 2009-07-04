#ifndef _LOADER_H
#define _LOADER_H

#include "uthash.h"
#include <tilesense.h>

typedef struct _genwrap {
  char *name;
  void *guts;
  UT_hash_handle hh;
} genwrap;

typedef genwrap * Hash;

struct _loader {
  char *path;
  Hash maps;
  TCOD_parser_t parser;
  void *listener;
//  Hash statuses;
//  Hash objectDefs;
};

typedef struct _loader * Loader;

Loader loader_new();
Loader loader_init(Loader l, char *basePath);
void loader_free(Loader l);

void loader_load_config(Loader l, char *configName); //init.config is automatically loaded
void loader_load_map(Loader l, char *mapName);
Map loader_get_map(Loader l, char *name);
void loader_load_status(Loader l, char *name);
void loader_load_object(Loader l, char *objType);
void loader_load_save(Loader l, char *saveName);

//for listeners only
void loader_add_map(Loader l, Map m, char *name);

#endif