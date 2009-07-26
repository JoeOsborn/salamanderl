#ifndef _EFFECT_PLACE_OBJECT_H
#define _EFFECT_PLACE_OBJECT_H

#include <tilesense.h>
#include "action/bindings.h"

struct _effect_place_object {
  char *object;
  bool definedPosition;
  mapVec position;
};

typedef struct _effect_place_object * EffectPlaceObject;

EffectPlaceObject effect_place_object_new();
EffectPlaceObject effect_place_object_init(EffectPlaceObject m, char *object, bool definedPosition, mapVec position);
void effect_place_object_free(EffectPlaceObject m);

void effect_place_object_request_bindings(EffectPlaceObject m, Bindings b);
void effect_place_object_apply(EffectPlaceObject m, Bindings b);

char *effect_place_object_id(EffectPlaceObject m, Bindings b);

#endif