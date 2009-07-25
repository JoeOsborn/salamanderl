#include "action/action.h"
#include "stdlib.h"
#include "string.h"

Action action_new() {
  return calloc(1, sizeof(struct _action));
}
Action action_init(Action a, char *label, 
  Flagset triggers, FlagSchema triggerSchema, 
  TCOD_list_t conditions, 
  TCOD_list_t grants, TCOD_list_t revokes, TCOD_list_t varsets, TCOD_list_t messages,
  EffectFeed feed, EffectPickUp pickup, EffectPutDown putdown, EffectGrab grab, EffectLetGo letgo) {
  a->bindings = bindings_init(bindings_new(), NULL, a->label ? a->label : "root", a, NULL);
  a->label = label ? strdup(label) : NULL;
  a->triggers = triggers;
  a->triggerSchema = triggerSchema;
  a->conditions = conditions ? conditions : TCOD_list_new();
  a->grants = grants ? grants : TCOD_list_new();
  a->revokes = revokes ? revokes : TCOD_list_new();
  a->varsets = varsets ? varsets : TCOD_list_new();
  a->messages = messages ? messages : TCOD_list_new();
  action_set_effect_feed(a, feed);
  action_set_effect_pick_up(a, pickup);
  action_set_effect_put_down(a, putdown);
  action_set_effect_grab(a, grab);
  action_set_effect_let_go(a, letgo);
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
  if(a->feed) {
    effect_feed_free(a->feed);
  }
  if(a->pickup) {
    effect_pick_up_free(a->pickup);
  }
  if(a->putdown) {
    effect_put_down_free(a->putdown);
  }
  if(a->grab) {
    effect_grab_free(a->grab);
  }
  if(a->letgo) {
    effect_let_go_free(a->letgo);
  }
  bindings_free(a->bindings);
  free(a);
}

EffectFeed action_effect_feed(Action a) {
  return a->feed;
}
void action_set_effect_feed(Action a, EffectFeed f) {
  a->feed = f;
  if(a->feed) {
    effect_feed_request_bindings(a->feed, a->bindings);
  }
}
EffectPickUp action_effect_pick_up(Action a) {
  return a->pickup;
}
void action_set_effect_pick_up(Action a, EffectPickUp p) {
  a->pickup = p;
  if(a->pickup) {
    effect_pick_up_request_bindings(a->pickup, a->bindings);
  }
}
EffectPutDown action_effect_put_down(Action a) {
  return a->putdown;
}
void action_set_effect_put_down(Action a, EffectPutDown p) {
  a->putdown = p;
  if(a->putdown) {
    effect_put_down_request_bindings(a->putdown, a->bindings);
  }
}
EffectGrab action_effect_grab(Action a) {
  return a->grab;
}
void action_set_effect_grab(Action a, EffectGrab g) {
  a->grab = g;
  if(a->grab) {
    effect_grab_request_bindings(a->grab, a->bindings);
  }
}
EffectLetGo action_effect_let_go(Action a) {
  return a->letgo;
}
void action_set_effect_let_go(Action a, EffectLetGo l) {
  a->letgo = l;
  if(a->letgo) {
    effect_let_go_request_bindings(a->letgo, a->bindings);
  }
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

  if(a->feed) {
    effect_feed_apply(a->feed, a->bindings);
  }
  if(a->pickup) {
    effect_pick_up_apply(a->pickup, a->bindings);
  }
  if(a->putdown) {
    effect_put_down_apply(a->putdown, a->bindings);
  }
  if(a->grab) {
    effect_grab_apply(a->grab, a->bindings);
  }
  if(a->letgo) {
    effect_let_go_apply(a->letgo, a->bindings);
  }

  //done!
  return true;
}
