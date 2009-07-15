#include "action/effect_set.h"
#include <string.h>
#include <stdlib.h>
#include <tilesense.h>

EffectSet effect_set_new() {
  return calloc(1, sizeof(struct _effect_set));
}
EffectSet effect_set_init(EffectSet v, SetMode mode, char *dstO, char *dstV, TCOD_value_t *value, char *srcO, char *srcV) {
  v->mode = mode;
  v->dstObject = strdup(dstO);
  v->dstVar = strdup(dstV);

  v->srcObject = srcO ? strdup(srcO) : strdup(dstO);
  v->srcVar = srcV ? strdup(srcV) : strdup(dstV);

  v->value = value; //if not present, src* are used.
  return v;
}
void effect_set_free(EffectSet v) {
  free(v->dstObject);
  free(v->dstVar);
  free(v->srcObject);
  free(v->srcVar);
  //free TCOD_value_t?
  free(v);
}

void effect_set_request_bindings(EffectSet v, Bindings b) {
  Bindings dst = bindings_insert(b, v->dstObject, NULL);
  bindings_insert(dst, v->dstVar, NULL);

  Bindings src = bindings_insert(b, v->srcObject, NULL);
  bindings_insert(src, v->srcVar, NULL);
}
//returns the specific bindings that were modified
Bindings effect_set_modify_bindings(EffectSet v, Bindings b) {
  Bindings src = bindings_find_path(b, v->srcObject);
  Bindings dst = bindings_find_path(b, v->dstObject);
  Bindings var = bindings_find_path(dst, v->dstVar);
  
  TCOD_value_t *lval = bindings_get_value_path(dst, v->dstVar);
  TCOD_value_t *rval = v->value ? v->value : bindings_get_value_path(src, v->srcVar);
  TCOD_value_t *newVal = calloc(1, sizeof(TCOD_value_t));
  if(!lval) { return NULL; } //bail, no lval!
  if(!rval) { return NULL; } //bail, no rval!

  float lnum = lval->f;
  float rnum = rval->f;
  int rnumi = rnum;
  char * lstr = lval->s;
  char * rstr = rval->s;
  TCOD_list_t ll = lval->list;
  TCOD_list_t rl = rval->list;
  
  switch(v->mode) {
    case Increase: newVal->f = lnum + rnum; break;
    case Decrease: newVal->f = lnum - rnum; break;
    case NumberSet: newVal->f = rnum; break;

    case Concat:
      asprintf(&(newVal->s), "%s%s", lstr, rstr);
      break;
    // case Excise:
    //   
    case StringSet:
      newVal->s = strdup(rstr);
      break;

    case Push:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, TCOD_list_push(newVal->list, each));
      TCOD_list_push(newVal->list, (void *)rnumi);
      break;
    case AppendAll:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, TCOD_list_push(newVal->list, each));
      TS_LIST_FOREACH(rl, TCOD_list_push(newVal->list, each));
      break;
    case Remove:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, 
        if((int)each == rnumi) { TCOD_list_push(newVal->list, each); }
      );
      break;
    case RemoveAll:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, 
        bool add = true;
        for(int j = 0; j < TCOD_list_size(rl); j++) {
          if((int)(TCOD_list_get(rl, j)) == (int)each) {
            add = false;
            break;
          }
        }
        if(add) { TCOD_list_push(newVal->list, each); }
      );
      break;
    case ListSet:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(rl, TCOD_list_push(newVal->list, each));
      break;
    
    case SPush:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, TCOD_list_push(newVal->list, strdup(each)));
      TCOD_list_push(newVal->list, strdup(rstr));
      break;
    case SAppendAll:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, TCOD_list_push(newVal->list, strdup(each)));
      TS_LIST_FOREACH(rl, TCOD_list_push(newVal->list, strdup(each)));
      break;
    case SRemove:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, 
        if(strcmp(each, rstr) != 0) { TCOD_list_push(newVal->list, each); }
      );
      break;
    case SRemoveAll:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(ll, 
        bool add = true;
        for(int j = 0; j < TCOD_list_size(rl); j++) {
          if(strcmp(TCOD_list_get(rl, j), each) == 0) {
            add = false;
            break;
          }
        }
        if(add) { TCOD_list_push(newVal->list, each); }
      );
      break;
    case SListSet:
      newVal->list = TCOD_list_new();
      TS_LIST_FOREACH(rl, TCOD_list_push(newVal->list, strdup(each)));
      break;
    default:
      free(newVal);
      newVal = NULL;
      break;
  }
  if(newVal) {
    bindings_set_value(var, newVal);
    return var;
  }
  return NULL;
}
