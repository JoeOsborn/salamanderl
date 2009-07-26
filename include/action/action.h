#ifndef _ACTION_H
#define _ACTION_H

#include <libtcod.h>
#include <tilesense.h>
#include "action/condition.h"
#include "action/bindings.h"
#include "action/effect_grantrevoke.h"
#include "action/effect_message.h"
#include "action/effect_set.h"
#include "action/effect_feed.h"
#include "action/effect_pick_up.h"
#include "action/effect_put_down.h"
#include "action/effect_grab.h"
#include "action/effect_let_go.h"
#include "action/effect_change_map.h"
#include "action/effect_place_object.h"

#include "status.h"

struct _action {
  char *label;
  //oh, the lengths we go to to inject these dependencies...
  FlagSchema triggerSchema;
  Flagset triggers;
  
  TCOD_list_t conditions;
  
  //these grants/revokes should be _copied_ when the action is activated!
  TCOD_list_t grants;
  TCOD_list_t revokes;
  
  TCOD_list_t varsets;
  
  TCOD_list_t messages;
  
  TCOD_list_t changeMaps;
  TCOD_list_t placeObjects;
  

  EffectFeed feed;
  EffectPickUp pickup;
  EffectPutDown putdown;
  EffectGrab grab;
  EffectLetGo letgo;

  //may also need owner data?  or is that only on invocations?
  
  Bindings bindings;
};

typedef struct _action *Action;


Action action_new();
Action action_init(Action a, char *label, 
  Flagset triggers, FlagSchema triggerSchema, 
  TCOD_list_t conditions, 
  TCOD_list_t grants, TCOD_list_t revokes, TCOD_list_t varsets, TCOD_list_t messages,
  TCOD_list_t changeMaps, TCOD_list_t placeObjects,
  EffectFeed feed, EffectPickUp pickup, EffectPutDown putdown, EffectGrab grab, EffectLetGo letgo);
void action_free(Action a);

void action_bind(Action a, Bindings b);
//these triggers _MUST_ be in the same schema as the action's schema for now.
bool action_apply(Action a, Flagset triggers);


EffectFeed action_effect_feed(Action a);
void action_set_effect_feed(Action a, EffectFeed f);
EffectPickUp action_effect_pick_up(Action a);
void action_set_effect_pick_up(Action a, EffectPickUp p);
EffectPutDown action_effect_put_down(Action a);
void action_set_effect_put_down(Action a, EffectPutDown p);
EffectGrab action_effect_grab(Action a);
void action_set_effect_grab(Action a, EffectGrab g);
EffectLetGo action_effect_let_go(Action a);
void action_set_effect_let_go(Action a, EffectLetGo l);
/*
actions, conditions, and actors:

action {
  //actors will get picked uniquely from nearby objects.
  //min_actor_range and max_actor_range may also be defined properties.
  //each combination of actors will be checked against the conditions.
  //an exception is made for self, which is always bound to the action evaluator.
  condition {
    //checks are done relative to object if reasonable, globally otherwise
    check "friendliness" {
      //target="object" could be used here if necessary, it's default.
      //target="self" could be used if you want to check your own properties.
      //do those defaults make sense?  should it be "object" for tile actions,
      //and "self" for object actions? the inconsistency could suck.
      greater_than=10 //"if friendliness is > 10"
      //other props include less_than, equal_to (for numbers),
      //starts_with, ends_with, find_string, is_string(for strings)
      //count_greater_than, count_less_than..., contains_number, contains_string, contains_all_numbers, contains_all_strings (for lists)
    }
  }
  grant "wet" {
    target="object" //(default) iffy how/when this gets resolved to a real object.
  }
  set "friendliness" {
    target="object" //default again
    decrease=2 //decrease a number by a floating point value
    //increase, set_number, set_string, set_string_list, set_number_list, append, append_string, append_all, append_all_strings, concatenate(string), remove, remove_string, remove_all, remove_all_strings, etc...
    //also from="" & from_object="" to set one object's var to another's var, if the
    //above things are used as flags instead of properties.
    //can values used here be stored from earlier condition checks/etc? that seems really hard to do without exploding the syntax.
  }
}

*/

#endif