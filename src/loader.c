#include "loader.h"
#include "drawinfo.h"
#include "moveinfo.h"
#include "objectinfo.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "loader/prop.h"
#include "loader/structrecord.h"

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
  flagschema_insert(l->triggerSchema, "on_exit", 1);
  flagschema_insert(l->triggerSchema, "on_bump", 1);
  flagschema_insert(l->triggerSchema, "on_atop", 1);
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

  l->mapParser = maplistener_init_parser(TCOD_parser_new(), l);  
  l->mapListener = maplistener_init(maplistener_new(), l);

  return l;
}

void loader_free(Loader l) {
  LOADER_DICT_FREE(l->maps, map_free);
  LOADER_DICT_FREE(l->statuses, status_free);  
  
  TCOD_parser_delete(l->configParser);
  TCOD_parser_delete(l->statusParser);
  TCOD_parser_delete(l->mapParser);
  
  configlistener_free(l->configListener);
  statuslistener_free(l->statusListener);
  maplistener_free(l->mapListener);
  
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
  return LOADER_DICT_GUTS(l->maps, name);
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
  return LOADER_DICT_GUTS(l->statuses, name);
}

void loader_load_object(Loader l, char *objType) {
  //hardcode player below for now

  //load the object, create the object def and context prototype
}
void loader_load_save(Loader l, char *saveName) {
  loader_load_map(l, "cage");
  loader_load_object(l, "player");

  //for now, just make a player
  
  Map m = loader_get_map(l, "cage");
  #warning make this an objectinfo with moveinfos and drawinfos.
  DrawInfo playerDraw = drawinfo_init(drawinfo_new(), 0, TCOD_white, TCOD_black, '@');
  TCOD_list_t moveFlags = TCOD_list_new();
  TCOD_list_push(moveFlags, moveflag_init(moveflag_new(), "normal", 1));
  TCOD_list_push(moveFlags, moveflag_init(moveflag_new(), "wet", 0));
  MoveInfo playerMove = moveinfo_init(moveinfo_new(), moveFlags);
  ObjectInfo oi = objectinfo_init(objectinfo_new(), l, NULL, playerMove);
  objectinfo_add_drawinfo(oi, playerDraw);
  
  //for now, we'll just make the player wet for a minute at start
  //objectinfo_grant(oi, grant_init(grant_new(), "wet", 60, 5, "naturally moist"));
  
  Object player = object_init(object_new(), 
    "@", 
    (mapVec){5, 1, 0}, 
    (mapVec){1, 0, 0},
    m,
    oi
  );
  map_add_object(m, player);
  // Sensor leftEye = sensor_init(sensor_new(), "left_eye",
  //   frustum_init(frustum_new(),
  //     mapvec_zero,
  //     (mapVec){1, -1, 0},
  //     1, 2,
  //     0, 10
  //   ),
  //   NULL
  // );
  // Sensor rightEye = sensor_init(sensor_new(), "right_eye",
  //   frustum_init(frustum_new(),
  //     mapvec_zero,
  //     (mapVec){1, 1, 0},
  //     1, 2,
  //     0, 10
  //   ),
  //   NULL
  // );
  Sensor basicSense = sensor_init(sensor_new(), "basic_sense",
    sphere_init(sphere_new(),
      mapvec_zero,
      8
    ),
    NULL
  );
  // object_add_sensor(player, leftEye);
  // object_add_sensor(player, rightEye);
  object_add_sensor(player, basicSense);
}

void loader_add_move_flag(Loader l, char *moveFlag) {
  TCOD_list_push(l->moveFlags, moveFlag);
}

FlagSchema loader_trigger_schema(Loader l) {
  return l->triggerSchema;
}
TCOD_list_t loader_move_flags(Loader l) {
  return l->moveFlags;
}
