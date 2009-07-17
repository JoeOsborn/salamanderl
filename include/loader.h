#ifndef _LOADER_H
#define _LOADER_H

#include "uthash.h"
#include <tilesense.h>

#include "status.h"
#include "loader/structrecord.h"

typedef struct _genwrap {
  char *name;
  void *guts;
  UT_hash_handle hh;
} genwrap;

typedef genwrap * Hash;

struct _loader {
  char *path;
  Hash maps;
  TCOD_parser_t configParser, statusParser, objectParser, mapParser, saveParser;
  void * configListener; //avoid recursive defs
  void * statusListener;
  void * objectListener;
  void * mapListener;
  void * saveListener;
  FlagSchema triggerSchema;
  TCOD_list_t moveFlags;
  Hash statuses;
  Hash objectDefs;
};

typedef struct _loader * Loader;

#include "loader/configlistener.h"
#include "loader/statuslistener.h"
#include "loader/objectlistener.h"
#include "loader/maplistener.h"
#include "loader/savelistener.h"

Loader loader_new();
Loader loader_init(Loader l, char *basePath);
void loader_free(Loader l);

void loader_load_config(Loader l, char *configName); //init.config is automatically loaded
void loader_load_map(Loader l, char *mapName);
Map loader_get_map(Loader l, char *name);
void loader_load_status(Loader l, char *name);
void loader_load_object(Loader l, char *objType);
void loader_load_save(Loader l, char *saveName);

Status loader_get_status(Loader l, char *name);

Flagset loader_make_trigger(Loader l, char *trigName);

//for listeners only
void loader_add_map(Loader l, Map m, char *name);

void loader_add_trigger(Loader l, char *name);
void loader_add_status(Loader l, Status s, char *name);
void loader_add_move_flag(Loader l, char *moveFlag);
void loader_add_object_def(Loader l, StructRecord sr);

FlagSchema loader_trigger_schema(Loader l);
TCOD_list_t loader_move_flags(Loader l);

Object loader_make_object(Loader l, StructRecord overrides);
#endif