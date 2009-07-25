#include "action/effect_grab.h"
#include <stdlib.h>
#include <string.h>

EffectGrab effect_grab_new() {
  return calloc(1, sizeof(struct _effect_grab));
}
EffectGrab effect_grab_init(EffectGrab m, char *grabber, char *grabbed) {
  m->grabber = strdup(grabber);
  m->grabbed = strdup(grabbed);
  return m;
}
void effect_grab_free(EffectGrab m) {
  free(m->grabber);
  free(m->grabbed);
  free(m);
}
void effect_grab_request_bindings(EffectGrab m, Bindings b) {
  bindings_insert(b, m->grabber, NULL);
  bindings_insert(b, m->grabbed, NULL);
}
void effect_grab_apply(EffectGrab m, Bindings b) {

}
