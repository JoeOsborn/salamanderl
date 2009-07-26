#include "action/effect_change_map.h"
#include "action/effect_place_object.h"
#include "loader.h"
#include <stdlib.h>
#include <string.h>

EffectChangeMap effect_change_map_new() {
  return calloc(1, sizeof(struct _effect_change_map));
}
EffectChangeMap effect_change_map_init(EffectChangeMap m, char *newMap, TCOD_list_t placeObjects) {
  m->newMap = strdup(newMap);
  m->placeObjects = placeObjects ? placeObjects : TCOD_list_new();
  return m;
}
void effect_change_map_free(EffectChangeMap m) {
  free(m->newMap);
  TS_LIST_CLEAR_AND_DELETE(m->placeObjects, effect_place_object);
  free(m);
}

void effect_change_map_request_bindings(EffectChangeMap m, Bindings b) {
  
}

void effect_change_map_apply(EffectChangeMap m, Bindings b) {
  //got to get the object from the map
  Map oldMap = bindings_get_value_path(b, "map");
  Loader l = bindings_get_value_path(b, "loader");
  Map newMap = loader_get_map(l, m->newMap);
  bindings_set_value_path(b, "map", newMap);
  //for each effect_place_object
  TS_LIST_FOREACH(m->placeObjects,
    char *id = effect_place_object_id(each, b);
    Object o = map_get_object_named(oldMap, id);
    //do this to avoid the case where you're moving from a large map to a small map and something could break.
    map_set_object_position(oldMap, id, mapvec_zero);
    //then go ahead and do the switch
    map_remove_object(oldMap, o);
    map_add_object(newMap, o);
    effect_place_object_apply(each, b);
  );
}
