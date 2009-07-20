#include "loader.h"
#include "drawinfo.h"
#include "moveinfo.h"
#include "objectinfo.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "loader/prop.h"

#include "loader/model_init_structrecord.h"

#define LOADER_DICT_FREE(_src, _freefunc) do { \
  while(_src) { \
    genwrap *__w = _src;  \
    _freefunc(__w->guts); \
    free(__w->name);  \
    HASH_DEL(_src, __w);  \
    free(__w);  \
  } \
} while(0)

#define LOADER_DICT_SET(_dest, _label, _guts) do { \
  genwrap *__w = loader_wrap(_label, _guts);  \
  HASH_ADD_KEYPTR(hh, _dest, __w->name, strlen(__w->name), __w);  \
} while(0)

//statements inside expression - evaluates to last statement.
//GCC extension.
#define LOADER_DICT_GUTS(_dst, _name) ({ \
  genwrap *_wrap=NULL; \
  HASH_FIND_STR(_dst, _name, _wrap); \
  _wrap ? _wrap->guts : NULL; \
})

char *makeRsrcPath(Loader l, char *subPath, char *suffix) {
  //basePath / subPath . suffix
  char *result;
  asprintf(&result, "%s/%s.%s", l->path, subPath, suffix);
  return result;
}

Loader loader_new() {
  return calloc(1, sizeof(struct _loader));
}

Loader loader_init(Loader l, char *basePath) {
  l->path = strdup(basePath);
  
  //first, install the basic trigger schema. this happens before any config files are loaded.
  l->triggerSchema = flagschema_init(flagschema_new());
  flagschema_insert(l->triggerSchema, "on_enter", 1);
  //pops every frame that A is inside of B
  flagschema_insert(l->triggerSchema, "on_inside", 1);
  flagschema_insert(l->triggerSchema, "on_exit", 1);
  flagschema_insert(l->triggerSchema, "on_bump", 1);
  flagschema_insert(l->triggerSchema, "on_atop", 1);
  flagschema_insert(l->triggerSchema, "on_walk_up", 1);
  flagschema_insert(l->triggerSchema, "on_walk_down", 1);
  flagschema_insert(l->triggerSchema, "on_fall_onto", 1);
  flagschema_insert(l->triggerSchema, "on_fall_through", 1);
  flagschema_insert(l->triggerSchema, "on_chomp", 1);
  flagschema_insert(l->triggerSchema, "on_unchomp", 1);
  flagschema_insert(l->triggerSchema, "on_tug_left", 1);
  flagschema_insert(l->triggerSchema, "on_tug_right", 1);
  flagschema_insert(l->triggerSchema, "on_tug_back", 1);
  
  l->configParser = configlistener_init_parser(TCOD_parser_new(), l);
  l->configListener = configlistener_init(configlistener_new(), l);
  
  l->moveFlags = TCOD_list_new();
  loader_add_move_flag(l, "normal");
  
  loader_load_config(l, "init");
  
  l->statusParser = statuslistener_init_parser(TCOD_parser_new(), l);
  l->statusListener = statuslistener_init(statuslistener_new(), l);
  
  TCOD_list_t statusFiles = configlistener_status_files(l->configListener);
  TS_LIST_FOREACH(statusFiles, loader_load_status(l, each));

  l->objectParser = objectlistener_init_parser(TCOD_parser_new(), l);  
  l->objectListener = objectlistener_init(objectlistener_new(), l);

  l->mapParser = maplistener_init_parser(TCOD_parser_new(), l);  
  l->mapListener = maplistener_init(maplistener_new(), l);

  l->saveParser = savelistener_init_parser(TCOD_parser_new(), l);
  l->saveListener = savelistener_init(savelistener_new(), l);

  return l;
}

void loader_free(Loader l) {
  LOADER_DICT_FREE(l->maps, map_free);
  LOADER_DICT_FREE(l->statuses, status_free);  
  
  TCOD_parser_delete(l->configParser);
  TCOD_parser_delete(l->statusParser);
  TCOD_parser_delete(l->objectParser);
  TCOD_parser_delete(l->mapParser);
  TCOD_parser_delete(l->saveParser);
  
  configlistener_free(l->configListener);
  statuslistener_free(l->statusListener);
  objectlistener_free(l->objectListener);
  maplistener_free(l->mapListener);
  maplistener_free(l->saveListener);
  
  TCOD_list_clear_and_delete(l->moveFlags);
  flagschema_free(l->triggerSchema);
}

void loader_load_config(Loader l, char *configName) {
  char *fileName = makeRsrcPath(l, configName, "config");
  
  TCOD_list_t evts = TCOD_parser_run_stax(l->configParser, fileName);
  configlistener_handle_events(l->configListener, evts);
  
  TCOD_list_clear_and_delete(evts);
  free(fileName);
}

void loader_load_map(Loader l, char *mapName) {  
  char *fileName = makeRsrcPath(l, mapName, "map");
  
  TCOD_list_t evts = TCOD_parser_run_stax(l->mapParser, fileName);
  maplistener_handle_events(l->mapListener, evts);
  
  TCOD_list_clear_and_delete(evts);
  free(fileName);
}

genwrap *loader_wrap(char *name, void *data) {
  genwrap *w = calloc(1, sizeof(genwrap));
  w->name=strdup(name);
  w->guts = data;
  return w;
}

void loader_add_map(Loader l, Map map, char *mapName) {
  LOADER_DICT_SET(l->maps, mapName, map);
}

Map loader_get_map(Loader l, char *name) {
  Map m = LOADER_DICT_GUTS(l->maps, name);
  if(!m) {
    loader_load_map(l, name);
    m = LOADER_DICT_GUTS(l->maps, name);
    if(!m) {
      return NULL;
    }
  }
  return m;
}

Flagset loader_make_trigger(Loader l, char *trigName) {
  Flagset fs = flagset_init(flagset_new(l->triggerSchema), l->triggerSchema);
  flagset_set_path(fs, l->triggerSchema, trigName, 1);
  return fs;
}

void loader_add_trigger(Loader l, char *trigName) {
  flagschema_insert(l->triggerSchema, trigName, 1);
}
void loader_add_status(Loader l, Status s, char *name) {
  LOADER_DICT_SET(l->statuses, name, s);
}

void loader_load_status(Loader l, char *name) {
  char *fileName = makeRsrcPath(l, name, "status");
  
  TCOD_list_t evts = TCOD_parser_run_stax(l->statusParser, fileName);
  statuslistener_handle_events(l->statusListener, evts);
  
  TCOD_list_clear_and_delete(evts);
  free(fileName);
}
Status loader_get_status(Loader l, char *name) {
  Status s = LOADER_DICT_GUTS(l->statuses, name);
  if(!s) {
    loader_load_status(l, name);
    s = LOADER_DICT_GUTS(l->statuses, name);
    if(!s) {
      return NULL;
    }
  }
  return s;
}

void loader_load_object(Loader l, char *objType) {
  char *fileName = makeRsrcPath(l, objType, "obj");
  
  TCOD_list_t evts = TCOD_parser_run_stax(l->objectParser, fileName);
  objectlistener_handle_events(l->objectListener, evts);
  
  TCOD_list_clear_and_delete(evts);
  free(fileName);
}
void loader_load_save(Loader l, char *saveName) {
  char *fileName = makeRsrcPath(l, saveName, "save");
  
  TCOD_list_t evts = TCOD_parser_run_stax(l->saveParser, fileName);
  savelistener_handle_events(l->saveListener, evts);
  
  TCOD_list_clear_and_delete(evts);
  free(fileName);
}

void loader_add_move_flag(Loader l, char *moveFlag) {
  TCOD_list_push(l->moveFlags, moveFlag);
}

void loader_add_object_def(Loader l, StructRecord sr) {
  char *n = structrecord_name(sr);
  LOADER_DICT_SET(l->objectDefs, n, sr);
}

Object loader_make_object(Loader l, StructRecord overrides) {
  char *name = structrecord_name(overrides);
  StructRecord def = LOADER_DICT_GUTS(l->objectDefs, name);
  if(def) {
    return object_init_structrecord_overrides(object_new(), l, def, overrides);
  } else {
    loader_load_object(l, name);
    if(LOADER_DICT_GUTS(l->objectDefs, name)) {
      return loader_make_object(l, overrides);
    }
  }
  return NULL;
}

FlagSchema loader_trigger_schema(Loader l) {
  return l->triggerSchema;
}
TCOD_list_t loader_move_flags(Loader l) {
  return l->moveFlags;
}
