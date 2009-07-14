#ifndef _CHECK_H
#define _CHECK_H

#include <libtcod.h>
#include "action/bindings.h"
#include "action/comparison.h"

struct _check {
  char *var;
  char *targ;
  
  char *cVar;
  char *cObj;
  TCOD_list_t comparisons;
};
typedef struct _check *Check;

Check check_new();
Check check_init(Check c, char *var, char *targ, char *cVar, char *cObj, TCOD_list_t comparisons);
void check_free(Check c);

void check_request_bindings(Check c, Bindings bindings);
bool check_result(Check c, Bindings bindings);

#endif