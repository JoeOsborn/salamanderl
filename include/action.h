#ifndef _ACTION_H
#define _ACTION_H

#include <libtcod.h>
#include <tilesense.h>
#include "condition.h"
#include "status.h"

//still have to:
/*
* load trigger schema from init.config
* tell loaderlistener how to handle actions and conditions
* roughly implement conditions in the loader
  * later, must decide how conditions like flag-checks will work
* send triggers appropriately during movement, etc.
* activate actions based on these triggers
* decrement timeLeft on grants appropriately -- cycle through all
    objects once in salamanderl.c's loop?
((side notes: could the crushable trapdoor in the cage be better implemented
as a door on the level below, removing the need for a trapdoor primitive?
also, it could be cute if pits and stairs both used movement-like flag checks
or full-on condition { }s.))
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
  
  //may also need owner data?  or is that only on invocations?
};

typedef struct _action *Action;

Action action_new();
Action action_init(Action a, char *label, Flagset triggers, FlagSchema triggerSchema, TCOD_list_t conditions, TCOD_list_t grants, TCOD_list_t revokes);
void action_free(Action a);

#endif