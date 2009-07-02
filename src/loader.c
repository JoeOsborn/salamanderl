#include "loader.h"
#include "drawinfo.h"

#include <stdlib.h>
#include <string.h>

Loader loader_new() {
  return calloc(1, sizeof(struct _loader));
}
Loader loader_init(Loader l, char *basePath) {
  l->path = calloc(strlen(basePath)+1, sizeof(char));
  strcpy(l->path, basePath);
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
  
}
void loader_load_config(Loader l, char *configName) {
  //don't even think about this yet
}
void loader_load_map(Loader l, char *mapName) {
  //hardcode for now
  unsigned short tileMap[] = {
    2,2,2,2,
    2,0,0,2,
    2,0,0,2,
    2,2,2,2,
    
    2,2,2,2,
    2,1,1,2,
    2,1,1,2,
    2,2,2,2
  };
  DrawInfo blankDraw = drawinfo_init(drawinfo_new(), TCOD_white, TCOD_black, ' ');
  DrawInfo floorDraw = drawinfo_init(drawinfo_new(), TCOD_white, TCOD_black, '.');
  DrawInfo wallDraw = drawinfo_init(drawinfo_new(), TCOD_white, TCOD_black, '#');
  Map map = map_init(map_new(), mapName, (mapVec){4, 4, 2}, tileMap, 8, NULL, blankDraw);
  Tile floorTile = tile_init(
    tile_new(), 
    0,
    floorDraw
  );
  Tile wallTile = tile_init(
    tile_new(), 
    3,
    wallDraw
  );
  map_add_tile(map, floorTile);
  map_add_tile(map, wallTile);

  genwrap *mapw = calloc(1, sizeof(genwrap));
  mapw->name = calloc(strlen(mapName)+1, sizeof(char));
  strcpy(mapw->name, mapName);
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
