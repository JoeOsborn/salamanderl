#include "action/effect_put_down.h"
#include <stdlib.h>
#include <string.h>
#include "objectinfo.h"

EffectPutDown effect_put_down_new() {
  return calloc(1, sizeof(struct _effect_put_down));
}
EffectPutDown effect_put_down_init(EffectPutDown m, char *carrier, char *carried) {
  m->carrier = strdup(carrier);
  m->carried = strdup(carried);
  return m;
}
void effect_put_down_free(EffectPutDown m) {
  free(m->carrier);
  free(m->carried);
  free(m);
}
void effect_put_down_request_bindings(EffectPutDown m, Bindings b) {
  bindings_insert(b, m->carrier, NULL);
  bindings_insert(b, m->carried, NULL);
}
void effect_put_down_apply(EffectPutDown m, Bindings b) {
  Object o = bindings_get_value_path(b, m->carrier);
  Object o2 = bindings_get_value_path(b, m->carried);
  objectinfo_detach(object_context(o), o, o2);
}
