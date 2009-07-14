#include "action/condition.h"

#include <stdlib.h>
#include <tilesense.h>

Condition condition_new() {
  return calloc(1, sizeof(struct _condition));
}
Condition condition_init(Condition c, BooleanMode mode, bool negate, TCOD_list_t checks, TCOD_list_t subconditions) {
  c->mode = mode;
  c->negate = negate;
  c->checks = checks ? checks : TCOD_list_new();
  c->subconditions = subconditions ? subconditions : TCOD_list_new();
  return c;
}
void condition_free(Condition c) {
  TS_LIST_CLEAR_AND_DELETE(c->checks, check);
  TS_LIST_CLEAR_AND_DELETE(c->subconditions, condition);
  free(c);
}

void condition_request_bindings(Condition c, Bindings bindings) {
  TS_LIST_FOREACH(c->checks, check_request_bindings(each, bindings));
  TS_LIST_FOREACH(c->subconditions, condition_request_bindings(each, bindings));
}

bool condition_result(Condition c, Bindings bindings) {
  bool result=false;
  TS_LIST_FOREACH(c->checks, 
    result = check_result(each, bindings);
    if(!result && (c->mode == BooleanAll)) { //a false result means and is false
      return (c->negate); //if negate is false, the result is false; true otherwise.
    }
    if(result && (c->mode == BooleanAny)) { //a true result means any is true
      return !(c->negate);
    }
  );
  TS_LIST_FOREACH(c->subconditions, 
    result = condition_result(each, bindings);
    if(!result && (c->mode == BooleanAll)) { //a false result means and is false
      return (c->negate); //if negate is false, the result is false; true otherwise.
    }
    if(result && (c->mode == BooleanAny)) { //a true result means any is true
      return !(c->negate);
    }
  );
  return !(c->negate); //return true by default, false if negated.
}