#include "action/effect_place_object.h"
#include <stdlib.h>
#include <string.h>

EffectPlaceObject effect_place_object_new() {
  return calloc(1, sizeof(struct _effect_place_object));
}
EffectPlaceObject effect_place_object_init(EffectPlaceObject m, char *object, bool definedPosition, mapVec position) {
  m->object = strdup(object);
  m->definedPosition = definedPosition;
  m->position = position;
  return m;
}
void effect_place_object_free(EffectPlaceObject m) {
  free(m->object);
  free(m);
}

void effect_place_object_request_bindings(EffectPlaceObject m, Bindings b) {
  
}
void effect_place_object_apply(EffectPlaceObject m, Bindings b) {
  //got to get the object from the map
  Map map = bindings_get_value_path(b, "map");
  char *id = effect_place_object_id(m, b);
  map_set_object_position(map, id, m->definedPosition ? m->position : object_position(map_get_object_named(map, id)));
}

char *effect_place_object_id(EffectPlaceObject m, Bindings b) {
  //is this an object in the bindings?  if so, use that object's id; otherwise, use this as-is
  Object *boundObj = bindings_get_value_path(b, m->object);
  if(boundObj) {
    return object_id(boundObj);
  }
  return m->object;
}