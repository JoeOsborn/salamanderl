#ifndef _OBJECT_INFO_H
#define _OBJECT_INFO_H

#include <libtcod.h>
#include "drawinfo.h"
#include "moveinfo.h"
#include "status.h"
#include "loader.h"

typedef enum {
  AttachNone,
  AttachCarry,
  AttachLatch
} AttachMode;

struct _object_info {
  //the object info needs to track the loader so it can
  //ask for statuses.
  Loader loader;
  TCOD_list_t drawinfos;
  TCOD_list_t actions;
  TCOD_list_t moveinfos;
  MoveInfo netMoveinfo;
  //a history of all grants and revocations of statuses.
  //should it have a maximum size?
  TCOD_list_t grants;
  TCOD_list_t revokes;
  //the currently active statuses.
  TCOD_list_t statuses;
    
  int weight; //grams
  
  TCOD_list_t stomach; //contains some Food, which has time eaten, digest time, volume
  
  Object attachedObject;
  AttachMode attachMode;
  
  char *description;
  
  bool falling, chomping, underwater;
};

typedef struct _object_info *ObjectInfo;

ObjectInfo objectinfo_new();
ObjectInfo objectinfo_init(ObjectInfo oi, Loader l, TCOD_list_t dis, MoveInfo mi, TCOD_list_t actions, int weight, char *description);
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

char *objectinfo_description(ObjectInfo oi);

void objectinfo_eat(ObjectInfo oi, Object food, float volume, float digestTime);
void objectinfo_attach(ObjectInfo oi, Object o2, AttachMode mode);
void objectinfo_detach(ObjectInfo oi, Object o2);

Object objectinfo_attached_object(ObjectInfo oi);
AttachMode objectinfo_attach_mode(ObjectInfo oi);

bool objectinfo_trigger(ObjectInfo oi, Object self, Object other, char *trig);

bool objectinfo_chomping(ObjectInfo oi);
void objectinfo_set_chomping(ObjectInfo oi, bool c);
bool objectinfo_falling(ObjectInfo oi);
void objectinfo_set_falling(ObjectInfo oi, bool f);
bool objectinfo_underwater(ObjectInfo oi);
void objectinfo_set_underwater(ObjectInfo oi, bool u);

#endif