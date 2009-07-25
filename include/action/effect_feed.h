#ifndef _EFFECT_FEED_H
#define _EFFECT_FEED_H

#include "action/bindings.h"
#include <tilesense.h>

struct _effect_feed {
  char *food;
  char *eater;
  float volume;
  float digestTime;
};
typedef struct _effect_feed *EffectFeed;

EffectFeed effect_feed_new();
EffectFeed effect_feed_init(EffectFeed m, float volume, float digestTime, char *eater, char *food);
void effect_feed_free(EffectFeed m);

void effect_feed_request_bindings(EffectFeed m, Bindings b);
void effect_feed_apply(EffectFeed m, Bindings b);

#endif