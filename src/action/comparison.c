#include "action/comparison.h"
#include <stdlib.h>
#include <string.h>
#include <tilesense.h>

Comparison comparison_new() {
  return calloc(1, sizeof(struct _comparison));
}
Comparison comparison_init(Comparison c, ComparisonMode mode, TCOD_value_t *val) {
  c->mode = mode;
  c->value = val;
  return c;
}
void comparison_free(Comparison c) {
  //warning, free TCOD_value_t?
  free(c);
}

bool comparison_expects_rval(Comparison c) {
  return c->value == NULL;
}

bool comparison_result(Comparison c, TCOD_value_t *lval, TCOD_value_t *rval) {
  if(rval == NULL || c->value != NULL) { rval = c->value; }
  if(rval==NULL) { return false; } //bail, no rvalue at all
  float lnum = lval->f;
  float rnum = rval->f;
  int rnumi = rnum;
  char * lstr = lval->s;
  char * rstr = rval->s;
  TCOD_list_t ll = lval->list;
  TCOD_list_t rl = rval->list;
  
  char *end, *left;
  int rlen, offset;
  
  switch(c->mode) {
    case GreaterThan: return lnum > rnum;
    case LessThan: return lnum < rnum;
    case EqualTo: return lnum == rnum;
    case GreaterThanOrEqualTo: return lnum >= rnum;
    case LessThanOrEqualTo: return lnum <= rnum;

    case StartsWith: return strstr(lstr, rstr) == lstr;
    case EndsWith:
      end = strchr(lstr, '\0')-1;
      rlen = strlen(rstr);
      if((end-lstr) < rlen) { return false; }
      for(left = end; left > lstr; left--) {
        offset = end-left;
        if(offset >= rlen) { return false; }
        if(*left != rstr[rlen-offset]) { return false; }
      }
      return true;
    case FindString: return strstr(lstr, rstr) != NULL;
    case IsString: return strcmp(lstr, rstr) == 0;

    case CountGreaterThan: return TCOD_list_size(ll) > rnum;
    case CountLessThan: return TCOD_list_size(ll) < rnum;
    case CountEqualTo: return TCOD_list_size(ll) == rnum;
    case CountGreaterThanOrEqualTo: return TCOD_list_size(ll) >= rnum;
    case CountLessThanOrEqualTo: return TCOD_list_size(ll) <= rnum;

    case ContainsNumber: return TCOD_list_contains(ll, (void *)rnumi);
    case ContainsAll:
      TS_LIST_FOREACH(rl, if(!TCOD_list_contains(ll, each)) { return false; } );
      return true;
    case ContainsString: 
      TS_LIST_FOREACH(ll, if(strcmp(each, rstr) == 0) { return true; } );
      return false;
    case ContainsAllString: 
      TS_LIST_FOREACH(ll, if(strcmp(each, rstr) != 0) { return false; } );
      return true;
    default:
      return false;
  }
}
