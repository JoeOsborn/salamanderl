#ifndef _OBJECT_INFO_H
#define _OBJECT_INFO_H

#include <libtcod.h>
#include "drawinfo.h"
#include "moveinfo.h"
#include "status.h"
#include "loader.h"

typedef enum {
  ChompNone,
  ChompEat,
  ChompCarry,
  ChompLatch
} ChompReaction;

struct _object_info {
  //the object info needs to track the loader so it can
  //ask for statuses.
  Loader loader;
  TCOD_list_t drawinfos;
  TCOD_list_t moveinfos;
  MoveInfo netMoveinfo;
  //a history of all grants and revocations of statuses.
  //should it have a maximum size?
  TCOD_list_t grants;
  TCOD_list_t revokes;
  //the currently active statuses.
  TCOD_list_t statuses;
  
  ChompReaction reaction;
  
  float foodVolume; //fraction of an average salamander's stomach capacity
  int digestionDuration; //seconds
  
  int weight; //grams
};

typedef struct _object_info *ObjectInfo;

ObjectInfo objectinfo_new();
ObjectInfo objectinfo_init(ObjectInfo oi, Loader l, TCOD_list_t dis, MoveInfo mi, ChompReaction reaction, float foodVolume, int digestionDuration, int weight);
void objectinfo_free(ObjectInfo oi);
void objectinfo_add_drawinfo(ObjectInfo oi, DrawInfo di);
TCOD_list_t objectinfo_drawinfos(ObjectInfo oi);
void objectinfo_set_moveinfo(ObjectInfo oi, MoveInfo mi);
MoveInfo objectinfo_moveinfo(ObjectInfo oi);
bool objectinfo_move_default_allowed(ObjectInfo oi);

bool objectinfo_status_active(ObjectInfo oi, Status s);
void objectinfo_grant(ObjectInfo oi, Grant g);
void objectinfo_revoke(ObjectInfo oi, Revoke r);
void objectinfo_apply_status(ObjectInfo oi, Status s);
void objectinfo_revoke_status(ObjectInfo oi, Status s);

void objectinfo_remake_net_moveinfo(ObjectInfo oi);

#endif