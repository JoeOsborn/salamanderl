#ifndef _EFFECT_SET_H
#define _EFFECT_SET_H

#include <libtcod.h>
#include "action/bindings.h"

typedef enum {
  SetNone,
  
  Increase,
  Decrease,
  NumberSet,

  Push,
  Remove,
  
  Concat,
  Excise, //not yet supported
  StringSet,
    
  SPush,
  SRemove,

  AppendAll,
  RemoveAll,
  ListSet,

  SAppendAll,
  SRemoveAll,
  SListSet
  
  //if from="" is defined, from_object will also be checked (defaults to same as target).  the SetMode should then be used as a -flag-, not a property. if it's used as a property, the from variable will be ignored.
} SetMode;

struct _effect_set {
  SetMode mode;
  char *dstObject;
  char *dstVar;
  TCOD_value_t *value; //if defined; else use:
  char *srcObject;
  char *srcVar;
};
typedef struct _effect_set *EffectSet;

SetMode effect_set_mode_from_name(char *n);

EffectSet effect_set_new();
EffectSet effect_set_init(EffectSet v, SetMode mode, char *dstV, char *dstO, char *srcV, char *srcO, TCOD_value_t *value);
void effect_set_free(EffectSet v);

void effect_set_request_bindings(EffectSet v, Bindings b);
//returns the specific bindings that were modified
Bindings effect_set_modify_bindings(EffectSet v, Bindings b);


#endif