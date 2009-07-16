#include "loader.h"
#include "drawinfo.h"
#include "moveinfo.h"
#include "objectinfo.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "loader/prop.h"
#include "loader/structrecord.h"
#include "loader/loaderlistener.h"

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

void loader_init_parser(Loader l) {
  l->parser = TCOD_parser_new();
  
  TCOD_parser_struct_t grantst = TCOD_parser_new_struct(l->parser, "grant");
  TCOD_struct_add_property(grantst, "duration", TCOD_TYPE_FLOAT, false); //defaults to infinity
  TCOD_struct_add_property(grantst, "priority", TCOD_TYPE_INT, false); //defaults to 1
  TCOD_struct_add_property(grantst, "reason", TCOD_TYPE_STRING, false); //defaults to NULL
  //this is actually only valid in grants {} on actions
  TCOD_struct_add_property(grantst, "target", TCOD_TYPE_STRING, false);

  TCOD_parser_struct_t revokest = TCOD_parser_new_struct(l->parser, "revoke");
  TCOD_struct_add_property(revokest, "priority", TCOD_TYPE_INT, false); //defaults to 1
  TCOD_struct_add_property(revokest, "reason", TCOD_TYPE_STRING, false); //defaults to NULL
  //this is actually only valid in revokes {} on actions
  TCOD_struct_add_property(revokest, "target", TCOD_TYPE_STRING, false);

  TCOD_parser_struct_t checkst = TCOD_parser_new_struct(l->parser, "check");
  TCOD_struct_add_property(checkst, "target", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "source", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "source_object", TCOD_TYPE_STRING, false);

  TCOD_struct_add_property(checkst, "greater_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "greater_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "less_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "less_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_flag(checkst, "greater_than");
  TCOD_struct_add_flag(checkst, "greater_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "less_than");
  TCOD_struct_add_flag(checkst, "less_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "equal_to");

  TCOD_struct_add_property(checkst, "starts_with", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "ends_with", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "find_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "is_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(checkst, "starts_with");
  TCOD_struct_add_flag(checkst, "ends_with");
  TCOD_struct_add_flag(checkst, "find_string");
  TCOD_struct_add_flag(checkst, "is_string");

  TCOD_struct_add_property(checkst, "count_greater_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_greater_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_less_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_less_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_flag(checkst, "count_greater_than");
  TCOD_struct_add_flag(checkst, "count_greater_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "count_less_than");
  TCOD_struct_add_flag(checkst, "count_less_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "count_equal_to");
  
  TCOD_struct_add_property(checkst, "contains", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(checkst, "contains_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(checkst, "contains_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(checkst, "contains_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(checkst, "contains");
  TCOD_struct_add_flag(checkst, "contains_string");
  TCOD_struct_add_flag(checkst, "contains_all");
  TCOD_struct_add_flag(checkst, "contains_all_strings");

  static const char *condition_modes[] = { "all", "any", NULL };    
  TCOD_parser_struct_t conditionst = TCOD_parser_new_struct(l->parser, "condition");
  TCOD_struct_add_value_list(conditionst, "require", condition_modes, false); //defaults to 'all'
  TCOD_struct_add_flag(conditionst, "negate");
  TCOD_struct_add_structure(conditionst, checkst);
  TCOD_struct_add_structure(conditionst, conditionst);  

  TCOD_parser_struct_t setst = TCOD_parser_new_struct(l->parser, "set");
  TCOD_struct_add_property(setst, "increase", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(setst, "decrease", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(setst, "set_number", TCOD_TYPE_FLOAT, false);
  //srcvar-using variants
  TCOD_struct_add_flag(setst, "increase");
  TCOD_struct_add_flag(setst, "decrease");
  TCOD_struct_add_flag(setst, "set_number");
  
  TCOD_struct_add_property(setst, "concat", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "excise", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "set_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(setst, "concat");
  TCOD_struct_add_flag(setst, "excise");
  TCOD_struct_add_flag(setst, "set_string");

  TCOD_struct_add_property(setst, "push", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "append_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(setst, "remove", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "remove_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "set_list", TCOD_TYPE_INT, false);  
  TCOD_struct_add_flag(setst, "push");
  TCOD_struct_add_flag(setst, "append_all");
  TCOD_struct_add_flag(setst, "remove");
  TCOD_struct_add_flag(setst, "remove_all");
  TCOD_struct_add_flag(setst, "set_list");

  TCOD_struct_add_property(setst, "push_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "append_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "remove_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "remove_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "set_string_list", TCOD_TYPE_STRING, false);  
  TCOD_struct_add_flag(setst, "push_string");
  TCOD_struct_add_flag(setst, "append_all_strings");
  TCOD_struct_add_flag(setst, "remove_string");
  TCOD_struct_add_flag(setst, "remove_all_strings");
  TCOD_struct_add_flag(setst, "set_string_list");
  

  TCOD_parser_struct_t actionst = TCOD_parser_new_struct(l->parser, "action");
  TCOD_list_t triggerLabels = flagschema_get_labels(l->triggerSchema);
  TS_LIST_FOREACH(triggerLabels,
    TCOD_struct_add_flag(actionst, each);
  );
  TCOD_list_clear_and_delete(triggerLabels);
  TCOD_struct_add_structure(actionst, conditionst);
  TCOD_struct_add_structure(actionst, grantst);  
  TCOD_struct_add_structure(actionst, revokest);  
  TCOD_struct_add_structure(actionst, setst);

  TCOD_parser_struct_t movst = TCOD_parser_new_struct(l->parser, "movement");
  TCOD_struct_add_property(movst, "normal", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(movst, "wet", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(movst, "ghost", TCOD_TYPE_BOOL, false);
  
  TCOD_parser_struct_t drawst = TCOD_parser_new_struct(l->parser, "draw");
  TCOD_struct_add_property(drawst, "z", TCOD_TYPE_INT, false); //defaults to the index of the drawst.
  TCOD_struct_add_property(drawst, "fore", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "back", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "symbol", TCOD_TYPE_CHAR, true);  

  static const char *movement_defaults[] = { "allow", "deny", NULL };    

  TCOD_parser_struct_t tst = TCOD_parser_new_struct(l->parser, "tile");
  //opacity
  TCOD_struct_add_list_property(tst, "opacity", TCOD_TYPE_CHAR, false); //defaults to [0,0,0,0,15,15,0,0] -- an opaque floor, no ceiling
  //opacity shorthands
  TCOD_struct_add_property(tst, "wall_opacity", TCOD_TYPE_CHAR, false); //defaults to 0
  TCOD_struct_add_property(tst, "floor_opacity", TCOD_TYPE_CHAR, false); //defaults to 15
  TCOD_struct_add_property(tst, "ceiling_opacity", TCOD_TYPE_CHAR, false); //defaults to 15
  TCOD_struct_add_property(tst, "uniform_opacity", TCOD_TYPE_CHAR, false); //defaults to 0

  //desc
  TCOD_struct_add_property(tst, "allow_desc", TCOD_TYPE_STRING, false); //defaults to ""
  TCOD_struct_add_property(tst, "deny_desc", TCOD_TYPE_STRING, false); //defaults to ""
  TCOD_struct_add_property(tst, "ontop_desc", TCOD_TYPE_STRING, false); //defaults to ""

  //stairs
  TCOD_struct_add_flag(tst, "stairs");
  TCOD_struct_add_property(tst, "up_desc", TCOD_TYPE_STRING, false); //defaults to ""
  TCOD_struct_add_property(tst, "down_desc", TCOD_TYPE_STRING, false); //defaults to ""

  //movement
  TCOD_struct_add_value_list(tst, "movement_default", movement_defaults, false); //defaults to "allow"
  TCOD_struct_add_structure(tst, movst);
  
  //actions
  TCOD_struct_add_structure(tst, actionst);  
  
  //drawing
  TCOD_struct_add_structure(tst, drawst);

  TCOD_parser_struct_t mapst = TCOD_parser_new_struct(l->parser, "map");
  TCOD_struct_add_property(mapst, "ambient_light", TCOD_TYPE_CHAR, false); //defaults to 8
  TCOD_struct_add_list_property(mapst, "dimensions", TCOD_TYPE_INT, true); //no default
  TCOD_struct_add_list_property(mapst, "tilemap", TCOD_TYPE_CHAR, true); //no default
  
}
Loader loader_init(Loader l, char *basePath) {
  l->path = strdup(basePath);

  loader_load_config(l, "init");

  loader_init_parser(l);

  l->mapListener = maplistener_init(maplistener_new(), l->triggerSchema, l);

  return l;
}

void loader_free(Loader l) {
  LOADER_DICT_FREE(l->maps, map_free);
  LOADER_DICT_FREE(l->statuses, status_free);  
  
  TCOD_parser_delete(l->parser);
}
void loader_load_config(Loader l, char *configName) {
  //don't even think about this yet
  l->triggerSchema = flagschema_init(flagschema_new());
  flagschema_insert(l->triggerSchema, "on_enter", 1);
  flagschema_insert(l->triggerSchema, "on_exit", 1);
  flagschema_insert(l->triggerSchema, "on_bump", 1);
  flagschema_insert(l->triggerSchema, "on_atop", 1);
  flagschema_insert(l->triggerSchema, "on_fall_onto", 1); //fall and land on ceiling
  flagschema_insert(l->triggerSchema, "on_fall_into", 1); //fall and land on floor
  flagschema_insert(l->triggerSchema, "on_fall_through", 1); //fall and keep falling
  flagschema_insert(l->triggerSchema, "on_chomp", 1);
  flagschema_insert(l->triggerSchema, "on_unchomp", 1);
  flagschema_insert(l->triggerSchema, "on_tug_left", 1);
  flagschema_insert(l->triggerSchema, "on_tug_right", 1);
  flagschema_insert(l->triggerSchema, "on_tug_back", 1);
  
  //timers and then custom triggers go here.  not sure what exactly to do for those.
}
Flagset loader_make_trigger(Loader l, char *trig) {
  Flagset ret = flagset_init(flagset_new(l->triggerSchema), l->triggerSchema);
  flagset_set_path(ret, l->triggerSchema, trig, 1);
  return ret;
}

void loader_load_map(Loader l, char *mapName) {  
  char *fileName = makeRsrcPath(l, mapName, "map");
  
  TCOD_list_t evts = TCOD_parser_run_stax(l->parser, fileName);
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
void loader_load_status(Loader l, char *name) {
  //don't even worry about this right now -- hardcode it
  TCOD_list_t moves = TCOD_list_new();
  TCOD_list_t wetFlags = TCOD_list_new();
  TCOD_list_push(wetFlags, moveflag_init(moveflag_new(), "wet", true));
  TCOD_list_push(moves, moveinfo_init(moveinfo_new(), wetFlags));
  Status wet = status_init(status_new(), "wet",
    NULL,
    NULL,
    moves,
    "You are soaking wet.",
    "You've become quite damp.",
    
    NULL,
    NULL,
    "Your skin is dry again."
  );
  LOADER_DICT_SET(l->statuses, "wet", wet);
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
  loader_load_status(l, "status");
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
