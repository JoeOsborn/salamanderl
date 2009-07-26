#ifndef _EFFECT_CHANGE_MAP_H
#define _EFFECT_CHANGE_MAP_H

#include <libtcod.h>
#include <tilesense.h>
#include "action/bindings.h"

struct _effect_change_map {
  char *newMap;
  TCOD_list_t placeObjects;
};

typedef struct _effect_change_map *EffectChangeMap;

EffectChangeMap effect_change_map_new();
EffectChangeMap effect_change_map_init(EffectChangeMap m, char *newMap, TCOD_list_t placeObjects);
void effect_change_map_free(EffectChangeMap m);

void effect_change_map_request_bindings(EffectChangeMap m, Bindings b);
void effect_change_map_apply(EffectChangeMap m, Bindings b);


#endif