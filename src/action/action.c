#include "action/action.h"
#include "stdlib.h"
#include "string.h"

Action action_new() {
  return calloc(1, sizeof(struct _action));
}
Action action_init(Action a, char *label, 
  Flagset triggers, FlagSchema triggerSchema, 
  TCOD_list_t conditions, 
  TCOD_list_t grants, TCOD_list_t revokes, TCOD_list_t varsets, TCOD_list_t messages) {
  a->label = label ? strdup(label) : NULL;
  a->triggers = triggers;
  a->triggerSchema = triggerSchema;
  a->conditions = conditions ? conditions : TCOD_list_new();
  a->grants = grants ? grants : TCOD_list_new();
  a->revokes = revokes ? revokes : TCOD_list_new();
  a->varsets = varsets ? varsets : TCOD_list_new();
  a->messages = messages ? messages : TCOD_list_new();
  a->bindings = bindings_init(bindings_new(), NULL, a->label ? a->label : "root", a, NULL);
  TS_LIST_FOREACH(a->conditions, condition_request_bindings(each, a->bindings));
  TS_LIST_FOREACH(a->grants, effect_grantrevoke_request_bindings(each, a->bindings));
  TS_LIST_FOREACH(a->revokes, effect_grantrevoke_request_bindings(each, a->bindings));
  TS_LIST_FOREACH(a->varsets, effect_set_request_bindings(each, a->bindings));
  TS_LIST_FOREACH(a->messages, effect_message_request_bindings(each, a->bindings));
  return a;
}
void action_free(Action a) {
  free(a->label);
  flagset_free(a->triggers);
  TS_LIST_CLEAR_AND_DELETE(a->conditions, condition);
  TS_LIST_CLEAR_AND_DELETE(a->grants, effect_grantrevoke);
  TS_LIST_CLEAR_AND_DELETE(a->revokes, effect_grantrevoke);
  TS_LIST_CLEAR_AND_DELETE(a->varsets, effect_set);
  TS_LIST_CLEAR_AND_DELETE(a->messages, effect_message);
  bindings_free(a->bindings);
  free(a);
}

void action_bind(Action a, Bindings b) {
  //clear bindings
  bindings_unbind(a->bindings);
  bindings_set_value(a->bindings, a);
  //fill based on given bindings
  bindings_fill_from(a->bindings, b);
  Bindings unbound=b;
  while((unbound = bindings_next_unbound(a->bindings))) {
    //no way to fill bindings yet!
    abort();
    //fill bindings from... where, exactly? should this get shuffled up a level?
  }
}
//these triggers _MUST_ be in the same schema as the action's schema for now.
bool action_apply(Action a, Flagset triggers) {
  //check triggers
  if(!flagset_any_match(a->triggers, triggers, a->triggerSchema)) { return false; }
  //check conditions
  TS_LIST_FOREACH(a->conditions, if(!condition_result(each, a->bindings)) { return false; });
  //set
  TCOD_list_t changedBindings = TCOD_list_new();
  TS_LIST_FOREACH(a->varsets, TCOD_list_push(changedBindings, effect_set_modify_bindings(each, a->bindings)));
  //now go through those changed bindings and actually update the object/etc data?
  //then clean up
  TCOD_list_delete(changedBindings);
  
  //revoke
  TS_LIST_FOREACH(a->revokes, effect_grantrevoke_revoke(each, a->bindings));
  //grant
  TS_LIST_FOREACH(a->grants, effect_grantrevoke_grant(each, a->bindings));

  TS_LIST_FOREACH(a->messages, effect_message_send(each, a->bindings));
  //done!
  return true;
}
