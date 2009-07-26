#include "action/effect_set.h"
#include <string.h>
#include <stdlib.h>
#include <tilesense.h>

SetMode effect_set_mode_from_name(char *n) {
  SetMode mode = SetNone;
  if(STREQ(n, "increase") || STREQ(n, "increase_src")) {
    mode = Increase;
  } else if(STREQ(n, "decrease") || STREQ(n, "decrease_src")) {
    mode = Decrease;
  } else if(STREQ(n, "set_number") || STREQ(n, "set_number_src")) {
    mode = NumberSet;
  } else if(STREQ(n, "concat") || STREQ(n, "concat_src")) {
    mode = Concat;
  } else if(STREQ(n, "excise") || STREQ(n, "excise_src")) {
    mode = Excise;
  } else if(STREQ(n, "set_string") || STREQ(n, "set_string_src")) {
    mode = StringSet;
  } else if(STREQ(n, "push") || STREQ(n, "push_src")) {
    mode = Push;
  } else if(STREQ(n, "append_all") || STREQ(n, "append_all_src")) {
    mode = AppendAll;
  } else if(STREQ(n, "remove") || STREQ(n, "remove_src")) {
    mode = Remove;
  } else if(STREQ(n, "remove_all") || STREQ(n, "remove_all_src")) {
    mode = RemoveAll;
  } else if(STREQ(n, "set_list") || STREQ(n, "set_list_src")) {
    mode = ListSet;
  } else if(STREQ(n, "push_string") || STREQ(n, "push_string_src")) {
    mode = SPush;
  } else if(STREQ(n, "append_all_strings") || STREQ(n, "append_all_strings_src")) {
    mode = SAppendAll;
  } else if(STREQ(n, "remove_string") || STREQ(n, "remove_string_src")) {
    mode = SRemove;
  } else if(STREQ(n, "remove_all_strings") || STREQ(n, "remove_all_strings_src")) {
    mode = SRemoveAll;
  } else if(STREQ(n, "set_string_list") || STREQ(n, "set_string_list_src")) {
    mode = SListSet;
  }
  return mode;
}

bool effect_set_string_rval(SetMode m) {
  return m >= Concat && m <= SRemove;
}
bool effect_set_list_rval(SetMode m) {
  return m >= AppendAll && m <= ListSet;
}
bool effect_set_string_list_rval(SetMode m) {
  return m >= SAppendAll && m <= SListSet;
}

EffectSet effect_set_new() {
  return calloc(1, sizeof(struct _effect_set));
}
EffectSet effect_set_init(EffectSet v, SetMode mode, char *dstV, char *dstO, char *srcV, char *srcO, TCOD_value_t *value) {
  v->mode = mode;
  v->dstObject = strdup(dstO);
  v->dstVar = strdup(dstV);

  v->srcObject = srcO ? strdup(srcO) : strdup(dstO);
  v->srcVar = srcV ? strdup(srcV) : strdup(dstV);
  
  if(value) {
    v->value = calloc(1, sizeof(TCOD_value_t));
    if(effect_set_string_rval(mode)) {
      v->value->s = value->s ? strdup(value->s) : "";
    } else if(effect_set_list_rval(mode)) {
      v->value->list = TCOD_list_duplicate(value->list);
    } else if(effect_set_string_list_rval(mode)) {
      v->value->list = TCOD_list_new();
      TS_LIST_FOREACH(value->list, TCOD_list_push(v->value->list, strdup(each)));
    } else {
      *(v->value) = *(value);
    }
  } else {
    v->value = NULL; //use src*
  }
  return v;
}
void effect_set_free(EffectSet v) {
  free(v->dstObject);
  free(v->dstVar);
  free(v->srcObject);
  free(v->srcVar);
  if(v->value) {
    if(effect_set_string_rval(v->mode)) {
      free(v->value->s);
    } else if(effect_set_list_rval(v->mode)) {
      TCOD_list_delete(v->value->list);
    } else if(effect_set_string_list_rval(v->mode)) {
      TCOD_list_clear_and_delete(v->value->list);
    }
    free(v->value);
  }
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
