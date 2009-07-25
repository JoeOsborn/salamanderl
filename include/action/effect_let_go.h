#ifndef _EFFECT_LET_GO_H
#define _EFFECT_LET_GO_H

#include "action/bindings.h"
#include <tilesense.h>

struct _effect_let_go {
  char *grabber;
  char *grabbed;
};
typedef struct _effect_let_go *EffectLetGo;

EffectLetGo effect_let_go_new();
EffectLetGo effect_let_go_init(EffectLetGo m, char *grabber, char *grabbed);
void effect_let_go_free(EffectLetGo m);

void effect_let_go_request_bindings(EffectLetGo m, Bindings b);
void effect_let_go_apply(EffectLetGo m, Bindings b);

#endif