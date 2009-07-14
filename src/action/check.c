#include "action/check.h"

#include <tilesense.h>
#include <stdlib.h>
#include <string.h>

Check check_new() {
  return calloc(1, sizeof(struct _check));
}
Check check_init(Check c, char *var, char *targ, char *cVar, char *cObj, TCOD_list_t comparisons) {
  c->var = strdup(var);
  c->targ = strdup(targ);
  c->cVar = cVar ? strdup(cVar) : strdup(var);
  c->cObj = cObj ? strdup(cObj) : strdup(targ);
  c->comparisons = comparisons ? comparisons : TCOD_list_new();
  return c;
}
void check_free(Check c) {
  free(c->var);
  free(c->targ);
  free(c->cVar);
  free(c->cObj);
  TS_LIST_CLEAR_AND_DELETE(c->comparisons, comparison);
  free(c);
}

void check_request_bindings(Check c, Bindings bindings) {
  Bindings targ = bindings_insert(bindings, c->targ, NULL);
  bindings_insert(targ, c->var, NULL);
  Bindings comp = bindings_insert(bindings, c->cObj, NULL);
  bindings_insert(comp, c->cVar, NULL);
}
bool check_result(Check c, Bindings bindings) {
  Bindings lObj = bindings_find_path(bindings, c->targ);
  TCOD_value_t *lVal = bindings_get_value_path(lObj, c->var);

  Bindings rObj = bindings_find_path(bindings, c->cObj);
  TCOD_value_t *defaultRVal = bindings_get_value_path(rObj, c->cVar);

  TS_LIST_FOREACH(c->comparisons,
    if(!comparison_result(each, lVal, comparison_expects_rval(each) ? defaultRVal : NULL)) {
      return false;
    }
  );
  return true;
}
