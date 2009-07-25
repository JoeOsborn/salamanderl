#ifndef _S_OBJ_H
#define _S_OBJ_H

#include <tilesense.h>
#include <libtcod.h>

void sobject_chomp(Object o);
void sobject_unchomp(Object o);
void sobject_presence_trigger(Object o, mapVec position, char *trig);
bool sobject_turn(Object o, int amt);

/*
when falling, the top pit gets on_enter, then fall_through, then on_exit.
the pit below that gets fall_into, on_enter, fall_through, on_exit
this repeats until:
  floor: fall_into, on_enter
  wall: fall_onto, atop
  pit at z=0: fall_into, on_enter
when walking normally, the old tile gets on_exit and the new gets on_enter
when walking up stairs, the old tile gets on_exit, the stairs get on_enter, on_walk_up, on_exit, on_atop, the square above the stairs gets on_enter
when walking down stairs, the old tile gets on_exit, the tile above the stairs gets on_enter, on_exit, the stairs get on_walk_down, on_enter.
when walking into a wall, only the wall's on_bump is triggered

this info might be good to have in a diagram
*/

void sobject_fall(Object o);
bool sobject_move(Object o, mapVec amt);
void sobject_input(Object player, TCOD_key_t key, float dt);
void sobject_update(Object o, float dt);

#endif