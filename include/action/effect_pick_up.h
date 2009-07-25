#ifndef _EFFECT_PICK_UP_H
#define _EFFECT_PICK_UP_H

#include "action/bindings.h"
#include <tilesense.h>

struct _effect_pick_up {
  char *carrier;
  char *carried;
};
typedef struct _effect_pick_up *EffectPickUp;

EffectPickUp effect_pick_up_new();
EffectPickUp effect_pick_up_init(EffectPickUp m, char *holder, char *held);
void effect_pick_up_free(EffectPickUp m);

void effect_pick_up_request_bindings(EffectPickUp m, Bindings b);
void effect_pick_up_apply(EffectPickUp m, Bindings b);

#endif