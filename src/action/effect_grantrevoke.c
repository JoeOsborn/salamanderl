#include "action/effect_grantrevoke.h"
#include <stdlib.h>
#include <string.h>

#include <tilesense.h>
#include "objectinfo.h"

EffectGrantRevoke effect_grantrevoke_new() {
  return calloc(1, sizeof(struct _effect_grantrevoke));
}
EffectGrantRevoke effect_grantrevoke_init(EffectGrantRevoke gr, void *effect, char *target) {
  gr->grant = effect; //fortunately, there's no difference in size between grant and revoke.
  gr->target = strdup(target);
  return gr;
}
void effect_grantrevoke_free(EffectGrantRevoke gr) {
  free(gr->target);
  free(gr);
}
void effect_grantrevoke_grant_free(EffectGrantRevoke gr) {
  grant_free(gr->grant);
  effect_grantrevoke_free(gr);
}
void effect_grantrevoke_revoke_free(EffectGrantRevoke gr) {
  revoke_free(gr->revoke);
  effect_grantrevoke_free(gr);
}

void effect_grantrevoke_request_bindings(EffectGrantRevoke gr, Bindings b) {
  bindings_insert(b, gr->target, NULL);
}
//be sure to use the right one for this action type.
void effect_grantrevoke_grant(EffectGrantRevoke gr, Bindings b) {
  Object o = bindings_get_value_path(b, gr->target);
  ObjectInfo oi = object_context(o);
  objectinfo_grant(oi, grant_copy(grant_new(), gr->grant));
}
void effect_grantrevoke_revoke(EffectGrantRevoke gr, Bindings b) {
  Object o = bindings_get_value_path(b, gr->target);
  ObjectInfo oi = object_context(o);
  objectinfo_revoke(oi, revoke_copy(revoke_new(), gr->revoke));
}
