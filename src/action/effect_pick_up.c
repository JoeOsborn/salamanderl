#include "action/effect_pick_up.h"
#include <stdlib.h>
#include <string.h>
#include "objectinfo.h"

EffectPickUp effect_pick_up_new() {
  return calloc(1, sizeof(struct _effect_pick_up));
}
EffectPickUp effect_pick_up_init(EffectPickUp m, char *carrier, char *carried) {
  m->carrier = strdup(carrier);
  m->carried = strdup(carried);
  return m;
}
void effect_pick_up_free(EffectPickUp m) {
  free(m->carrier);
  free(m->carried);
  free(m);
}
void effect_pick_up_request_bindings(EffectPickUp m, Bindings b) {
  bindings_insert(b, m->carrier, NULL);
  bindings_insert(b, m->carried, NULL);
}
void effect_pick_up_apply(EffectPickUp m, Bindings b) {
  Object o = bindings_get_value_path(b, m->carrier);
  Object o2 = bindings_get_value_path(b, m->carried);
  objectinfo_attach(object_context(o), o2, AttachCarry);
}
