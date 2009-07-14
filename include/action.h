#ifndef _ACTION_H
#define _ACTION_H

struct _action {
  Flagset triggers;
  TCOD_list_t grantStatuses;
  TCOD_list_t revokeStatuses;
};

typedef struct _action *Action;

Action action_new();
Action action_init(Action a, ActionTrigger trigger, );

#endif