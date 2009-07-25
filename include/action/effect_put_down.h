#ifndef _EFFECT_PUT_DOWN_H
#define _EFFECT_PUT_DOWN_H

#include "action/bindings.h"
#include <tilesense.h>

struct _effect_put_down {
  char *carrier;
  char *carried;
};
typedef struct _effect_put_down *EffectPutDown;

EffectPutDown effect_put_down_new();
EffectPutDown effect_put_down_init(EffectPutDown m, char *holder, char *held);
void effect_put_down_free(EffectPutDown m);

void effect_put_down_request_bindings(EffectPutDown m, Bindings b);
void effect_put_down_apply(EffectPutDown m, Bindings b);

#endif