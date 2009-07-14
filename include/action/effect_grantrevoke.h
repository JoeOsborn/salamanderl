#ifndef _EFFECT_GR_H
#define _EFFECT_GR_H

#include "action/bindings.h"
#include "status.h"

struct _effect_grantrevoke {
  union {
    Grant grant;
    Revoke revoke;
  };
  char *target;
};
typedef struct _effect_grantrevoke EffectGrantRevoke;

EffectGrantRevoke effect_grantrevoke_new();
EffectGrantRevoke effect_grantrevoke_init(EffectGrantRevoke gr, void *effect, char *target);
void effect_grantrevoke_free(EffectGrantRevoke gr);
void effect_grantrevoke_grant_free(EffectGrantRevoke gr);
void effect_grantrevoke_revoke_free(EffectGrantRevoke gr);

void effect_grantrevoke_request_bindings(EffectGrantRevoke gr, Bindings b);
//be sure to use the right one for this action type.
void effect_grantrevoke_grant(EffectGrantRevoke gr, Bindings b);
void effect_grantrevoke_revoke(EffectGrantRevoke gr, Bindings b);

#endif