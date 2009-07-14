#ifndef _CONDITION_H
#define _CONDITION_H

#include <libtcod.h>

struct _condition {
  bool any; //if false, interpret as "all" for nested conditions
  TCOD_list_t subconditions;
  
  //need the actual checks in here somehow
};
typedef struct _condition *Condition;
Condition condition_new();
Condition condition_init(Condition c, TCOD_list_t subconditions, bool any);
void condition_free(Condition c);

#endif