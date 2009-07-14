#ifndef _CONDITION_H
#define _CONDITION_H

#include <libtcod.h>
#include "action/bindings.h"
#include "action/check.h"
#include "action/comparison.h"

typedef enum {
  BooleanAny,
  BooleanAll
} BooleanMode;

struct _condition {
  BooleanMode mode;
  bool negate;
  TCOD_list_t checks;
  TCOD_list_t subconditions;
};
typedef struct _condition *Condition;
Condition condition_new();
Condition condition_init(Condition c, BooleanMode mode, bool negate, TCOD_list_t checks, TCOD_list_t subconditions);
void condition_free(Condition c);

void condition_request_bindings(Condition c, Bindings bindings);

bool condition_result(Condition c, Bindings bindings);

#endif