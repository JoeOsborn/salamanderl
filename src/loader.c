#include "loader.h"
#include "drawinfo.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "loader/prop.h"
#include "loader/structrecord.h"
#include "loader/loaderlistener.h"

char *makeRsrcPath(Loader l, char *subPath, char *suffix) {
  //basePath / subPath . suffix
  return asprintf("%s/%s.%s", l->basePath, subPath, suffix);
}

void loader_add_map(Loader l, Map map, char *mapName);

Loader loader_new() {
  return calloc(1, sizeof(struct _loader));
}

void loader_init_parser(Loader l) {
  l->parser = TCOD_parser_new();
  
  TCOD_parser_struct_t grantst = TCOD_parser_new_struct(l->parser, "grant");
  TCOD_struct_add_property(grantst, "duration", TCOD_TYPE_FLOAT, false); //defaults to 1 min

  TCOD_parser_struct_t revokest = TCOD_parser_new_struct(l->parser, "revoke");
//  TCOD_struct_add_flag(revokest, ""); ??

  TCOD_parser_struct_t actionst = TCOD_parser_new_struct(l->parser, "action");
  TCOD_struct_add_flag(actionst, "on_enter");
  TCOD_struct_add_structure(actionst, grantst);  
  TCOD_struct_add_structure(actionst, revokest);  

  TCOD_parser_struct_t movst = TCOD_parser_new_struct(l->parser, "movement");
  TCOD_struct_add_property(movst, "normal", TCOD_TYPE_BOOL, false);
  TCOD_struct_add_property(movst, "wet", TCOD_TYPE_BOOL, false);

  static const char *movement_defaults[] = { "allow", "deny", NULL };    
  TCOD_parser_struct_t tst = TCOD_parser_new_struct(l->parser, "tile");
  TCOD_struct_add_property(tst, "opacity", TCOD_TYPE_CHAR, false); //defaults to 0
  TCOD_struct_add_property(tst, "fore", TCOD_TYPE_COLOR, false); //defaults to white
  TCOD_struct_add_property(tst, "back", TCOD_TYPE_COLOR, false); //defaults to black
  TCOD_struct_add_property(tst, "symbol", TCOD_TYPE_CHAR, true); //no default
  TCOD_struct_add_valuelist(tst, "movement_default", movement_defaults, false); //defaults to "allow"
  TCOD_struct_add_structure(tst, movst);
  TCOD_struct_add_structure(tst, actionst);  

  TCOD_parser_new_struct(l->parser, "map");
  TCOD_struct_add_property(tst, "ambient_light", TCOD_TYPE_CHAR, false); //defaults to 8
  TCOD_struct_add_list_property(tst, "dimensions", TCOD_TYPE_INT, true); //no default
  TCOD_struct_add_list_property(tst, "tilemap", TCOD_TYPE_CHAR, true); //no default
  
}
Loader loader_init(Loader l, char *basePath) {
  l->path = strdup(basePath);
  l->listener = loader_listener_init(loader_listener_new(), l);
  loader_init_parser(l);

  loader_load_config(l, "init");
  return l;
}
void loader_free(Loader l) {
  genwrap *gw;

  while(l->maps) {
    gw = l->maps;
    map_free(gw->guts);
    free(gw->name);
    HASH_DEL(l->maps,gw);
    free(gw);
  }
  
  TCOD_parser_delete(l->parser);
}
void loader_load_config(Loader l, char *configName) {
  //don't even think about this yet
}

void loader_load_map(Loader l, char *mapName) {  
  fileName = makeRsrcPath(l, mapName, "map");
  
  TCOD_parser_run(l->parser, fileName, loader_listener_listencallbacks(l->listener));
  free(fileName);
  //parser ran and updated ctx -- ctx stored the records while they were being built, one at a time, and added them to us.
  //the next call to loader_get_map will return the map.
}

void loader_add_map(Loader l, Map map, char *mapName) {
  genwrap *mapw = calloc(1, sizeof(genwrap));
  mapw->name=strdup(mapName);
  mapw->guts = map;

  HASH_ADD_KEYPTR(hh, l->maps, mapw->name, strlen(mapw->name), mapw);  
}

Map loader_get_map(Loader l, char *name) {
  genwrap *mapw=NULL;
  HASH_FIND_STR(l->maps, name, mapw);  
  return mapw ? mapw->guts : NULL;
}
void loader_load_status(Loader l, char *name) {
  //don't even worry about this right now
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
  DrawInfo playerDraw = drawinfo_init(drawinfo_new(), TCOD_white, TCOD_black, '@');
  Object player = object_init(object_new(), 
    "@", 
    (mapVec){1, 1, 0}, 
    (mapVec){1, 0, 0},
    m,
    playerDraw
  );
  map_add_object(m, player);
  Sensor leftEye = sensor_init(sensor_new(), "left_eye",
    frustum_init(frustum_new(),
      mapvec_zero,
      (mapVec){1, -1, 0},
      1, 2,
      0, 10
    ),
    NULL
  );
  Sensor rightEye = sensor_init(sensor_new(), "right_eye",
    frustum_init(frustum_new(),
      mapvec_zero,
      (mapVec){1, 1, 0},
      1, 2,
      0, 10
    ),
    NULL
  );
  Sensor basicSense = sensor_init(sensor_new(), "basic_sense",
    sphere_init(sphere_new(),
      mapvec_zero,
      2
    ),
    NULL
  );
  object_add_sensor(player, leftEye);
  object_add_sensor(player, rightEye);
  object_add_sensor(player, basicSense);
}
