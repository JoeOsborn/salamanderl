#include "condition.h"
#include <stdlib.h>
#include <tilesense.h>

Condition condition_new() {
  return calloc(1, sizeof(struct _condition));
}
Condition condition_init(Condition c, TCOD_list_t subconditions, bool any) {
  c->subconditions = subconditions;
  c->any = any;
  //etc
  return c;
}
void condition_free(Condition c) {
  TS_LIST_CLEAR_AND_DELETE(c->subconditions, condition);
  free(c);
}