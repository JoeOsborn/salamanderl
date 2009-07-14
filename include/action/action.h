#ifndef _ACTION_H
#define _ACTION_H

#include <libtcod.h>
#include <tilesense.h>
#include "action/condition.h"
#include "action/bindings.h"
#include "action/effect_grantrevoke.h"
#include "action/effect_set.h"

#include "status.h"


//lots of stuff below.  but note: I'm under no obligation to include every good idea right now!  I should probably knuckle down and get something basic working for actions, first.

/*
conditions and actors.  this is probably how the product will end up, though we don't need to go all the way to this right now.

action {
  actors=["self", "object"] //the default.
  //actors will get picked uniquely from nearby objects.
  //min_actor_range and max_actor_range are defined properties.
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
      //starts_with, ends_with, find_string, is(for strings), is_any_case
      //count, contains_number, contains_string, contains_all (for lists)
    }
  }
  grant "wet" {
    target="object" //(default) iffy how/when this gets resolved to a real object.
  }
  set "friendliness" {
    target="object" //default again
    decrease=2 //decrease a number by a floating point value
    //increase, number=, string=, list=, push, append(list), concatenate(string), remove(list), remove_string(string), remove_all(list), etc...
    //also from="" & from_object="" to set one object's var to another's var, if the
    //above things are used as flags instead of properties.
    //can values used here be stored from earlier condition checks/etc? that seems really hard to do without exploding the syntax.
  }
}

*/

/*
* load trigger schema from init.config (CAN DEFER -- predefined trigger types sufficient for now)
* tell loaderlistener how to handle actions and conditions in tiles
    (really, devise *_init_structrecord for action, condition, grant, revoke, and add condition {} to the parser)
    for conditions, it's okay to just load the any flag and subconditions for now.
    later, must decide how conditions like flag-checks will work.
    perhaps a send_trigger="blah" would be good. multiple could be provided within an action.
      triggers may need senders.  this would work well for movements.
      would triggers be sent to absolutely every object, or to specific objects?
      how, in general, will objects be identified in actions? self/other is one option.
      
      by the way, in case I forget, doors:
        Option A: doors are objects -- they may be defined as tiles in the file format, but they'll get transformed into objects on our end.  this implies that objects must be able to block light.
        Option B: doors are tiles, and the map's tiles can change.  this seems quite feasible, provided the door tileinfo holds both old and new states.  maybe this can be held in properties of an action or something, 
      also, before I forget about statuses:
        drawinfos can have a "status=X" flag to request display when the owner has a given status.  right?  or have a generic/implicit condition { any=false } block that can check status, z-offset, whatever?
      moveinfos:
        should moveinfos be replaced with condition {}s? seems unnecessary but might reduce duplication, be nice for consistency. they already define an implicit condition { any=false } block.  replace with condition "enter" { any=true, condition {}, condition {}, ... }, condition "atop" {...}, condition "fall_through" {...}....
          if this is done, moveinfo and the movement_default flag can be replaced with this behavior:
            condition "enter" {
              any
              condition { false } //for move_default=false; "true" for true.
              condition { ghost=true } //or something
            }
            the question is whether this consistency is better than the convenience of the special case. it -does- mean that moveinfo checks can potentially be much, much cleverer
            
      conditions:
        now that I think about it, "any=t/f" sucks. how about "require=any/all" where the default is all (or an 'any' flag and an 'all' flag and the last takes precedence), and also a "not" flag that negates the result?

* send triggers appropriately during movement, etc.
    
* activate actions based on these triggers
* decrement timeLeft on grants appropriately -- cycle through all
    objects once in salamanderl.c's loop?
((side notes: could the crushable trapdoor in the cage be better implemented
as a door on the level below, removing the need for a trapdoor primitive?
also, it could be cute if pits and stairs both used movement-like flag checks
or full-on condition { }s.))

* actually load statuses from file (init.conf loader, .status loader)
  (may require a new loaderlistener -- consider renaming loaderlistener to
   maplistener.  set up a central file for *_init_structrecord?  factor out
   parser data type setup somewhere? NOTE that map parser data types must be
   set up -after- the init.config has set up the trigger types.)
*/

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
  //may also need owner data?  or is that only on invocations?
  
  Bindings bindings;
};

typedef struct _action *Action;


Action action_new();
Action action_init(Action a, char *label, Flagset triggers, FlagSchema triggerSchema, TCOD_list_t conditions, TCOD_list_t grants, TCOD_list_t revokes, TCOD_list_t varsets);
void action_free(Action a);

void action_bind(Action a, Bindings b);
//these triggers _MUST_ be in the same schema as the action's schema for now.
bool action_apply(Action a, Flagset triggers, Bindings b);

#endif