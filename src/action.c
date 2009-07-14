#include "action.h"
#include <stdlib.h>

Action action_new() {
  return calloc(1, sizeof(struct _action));
}
Action action_init(Action a, char *label, Flagset triggers, FlagSchema triggerSchema, TCOD_list_t conditions, TCOD_list_t grants, TCOD_list_t revokes) {
  a->label = label ? strdup(label) : NULL;
  a->triggers = triggers;
  a->triggerSchema = triggerSchema;
  a->conditions = conditions;
  a->grants = grants;
  a->revokes = revokes;
  return a;
}

void action_free(Action a) {
  if(a->label) { free(a->label); }
  flagset_free(a->triggers);
  TS_LIST_CLEAR_AND_DELETE(a->conditions, condition);
  TS_LIST_CLEAR_AND_DELETE(a->grants, grant);
  TS_LIST_CLEAR_AND_DELETE(a->revokes, revoke);
  free(a);
}
