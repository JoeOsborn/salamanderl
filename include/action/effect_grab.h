#ifndef _EFFECT_GRAB_H
#define _EFFECT_GRAB_H

#include "action/bindings.h"
#include <tilesense.h>

struct _effect_grab {
  char *grabber;
  char *grabbed;
};
typedef struct _effect_grab *EffectGrab;

EffectGrab effect_grab_new();
EffectGrab effect_grab_init(EffectGrab m, char *grabber, char *grabbed);
void effect_grab_free(EffectGrab m);

void effect_grab_request_bindings(EffectGrab m, Bindings b);
void effect_grab_apply(EffectGrab m, Bindings b);

#endif