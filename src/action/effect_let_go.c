#include "action/effect_let_go.h"
#include <stdlib.h>
#include <string.h>
#include "objectinfo.h"

EffectLetGo effect_let_go_new() {
  return calloc(1, sizeof(struct _effect_let_go));
}
EffectLetGo effect_let_go_init(EffectLetGo m, char *grabber, char *grabbed) {
  m->grabber = strdup(grabber);
  m->grabbed = strdup(grabbed);
  return m;
}
void effect_let_go_free(EffectLetGo m) {
  free(m->grabber);
  free(m->grabbed);
  free(m);
}
void effect_let_go_request_bindings(EffectLetGo m, Bindings b) {
  bindings_insert(b, m->grabber, NULL);
  bindings_insert(b, m->grabbed, NULL);
}
void effect_let_go_apply(EffectLetGo m, Bindings b) {
  Object o = bindings_get_value_path(b, m->grabber);
  Object o2 = bindings_get_value_path(b, m->grabbed);
  objectinfo_detach(object_context(o), o, o2);
}
