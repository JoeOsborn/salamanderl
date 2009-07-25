#include "action/effect_feed.h"
#include <stdlib.h>
#include <string.h>

EffectFeed effect_feed_new() {
  return calloc(1, sizeof(struct _effect_feed));
}
EffectFeed effect_feed_init(EffectFeed m, float vol, float time, char *eater, char *food) {
  m->volume = vol;
  m->digestTime = time;
  m->eater = strdup(eater);
  m->food = strdup(food);
  return m;
}
void effect_feed_free(EffectFeed m) {
  free(m->eater);
  free(m->food);
  free(m);
}
void effect_feed_request_bindings(EffectFeed m, Bindings b) {
  bindings_insert(b, m->eater, NULL);
  bindings_insert(b, m->food, NULL);
}
void effect_feed_apply(EffectFeed m, Bindings b) {

}
