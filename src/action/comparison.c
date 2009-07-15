#include "action/comparison.h"
#include <stdlib.h>
#include <string.h>
#include <tilesense.h>

ComparisonMode comparison_mode_from_name(char *n) {
  ComparisonMode comparisonMode = ComparisonNone;
  if(STREQ(n, "greater_than")) {
    comparisonMode = GreaterThan;
  } else if(STREQ(n, "greater_than_or_equal_to")) {
    comparisonMode = GreaterThanOrEqualTo;
  } else if(STREQ(n, "less_than")) {
    comparisonMode = LessThan;
  } else if(STREQ(n, "less_than_or_equal_to")) {
    comparisonMode = LessThanOrEqualTo;
  } else if(STREQ(n, "equal_to")) {
    comparisonMode = EqualTo;
  } else if(STREQ(n, "count_greater_than")) {
    comparisonMode = CountGreaterThan;
  } else if(STREQ(n, "count_greater_than_or_equal_to")) {
    comparisonMode = CountGreaterThanOrEqualTo;
  } else if(STREQ(n, "count_less_than")) {
    comparisonMode = CountLessThan;
  } else if(STREQ(n, "count_less_than_or_equal_to")) {
    comparisonMode = CountLessThanOrEqualTo;
  } else if(STREQ(n, "count_equal_to")) {
    comparisonMode = CountEqualTo;
  } else if(STREQ(n, "starts_with")) {
    comparisonMode = StartsWith;
  } else if(STREQ(n, "ends_with")) {
    comparisonMode = EndsWith;
  } else if(STREQ(n, "find_string")) {
    comparisonMode = FindString;
  } else if(STREQ(n, "is_string")) {
    comparisonMode = IsString;
  } else if(STREQ(n, "contains_number")) {
    comparisonMode = ContainsNumber;
  } else if(STREQ(n, "contains_string")) {
    comparisonMode = ContainsString;
  } else if(STREQ(n, "contains_all")) {
    comparisonMode = ContainsAll;
  } else if(STREQ(n, "contains_all_strings")) {
    comparisonMode = ContainsAllString;
  }
  return comparisonMode;
}

bool comparison_string_rval(ComparisonMode mode) {
  return (mode >= StartsWith && mode <= ContainsString); 
}

bool comparison_list_rval(ComparisonMode mode) {
  return (mode >= ContainsAll && mode <= ContainsAllString);
}

bool comparison_string_list_rval(ComparisonMode mode) {
  return (mode == ContainsAllString);
}

Comparison comparison_new() {
  return calloc(1, sizeof(struct _comparison));
}
Comparison comparison_init(Comparison c, ComparisonMode mode, TCOD_value_t *val) {
  c->mode = mode;
  if(val) {
    //make a copy
    c->value = calloc(1, sizeof(TCOD_value_t));
    if(comparison_string_rval(c->mode)) {
      c->value->s = val->s ? strdup(val->s) : "";
    } else if(comparison_list_rval(c->mode)) {
      c->value->list = TCOD_list_duplicate(val->list);
    } else if(comparison_string_list_rval(c->mode)) {
      c->value->list = TCOD_list_new();
      TS_LIST_FOREACH(val->list, TCOD_list_push(c->value->list, strdup(each)));
    } else {
      *(c->value) = *(val);
    }
  } else {
    c->value = NULL;
  }
  return c;
}
void comparison_free(Comparison c) {
  if(c->value) {
    if(comparison_string_rval(c->mode)) {
      free(c->value->s);
    } else if(comparison_list_rval(c->mode)) {
      TCOD_list_delete(c->value->list);
    } else if(comparison_string_list_rval(c->mode)) {
      TCOD_list_clear_and_delete(c->value->list);
    }
    free(c->value);
  }
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
