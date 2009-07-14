#ifndef _OBJECT_INFO_H
#define _OBJECT_INFO_H

#include <libtcod.h>
#include "drawinfo.h"
#include "moveinfo.h"
#include "status.h"

struct _object_info {
  //the object info needs to track the loader so it can
  //ask for statuses.
  Loader loader;
  TCOD_list_t drawinfos;
  TCOD_list_t moveinfos;
  MoveInfo netMoveinfo;
  TCOD_list_t statuses;
};

typedef struct _object_info *ObjectInfo;

ObjectInfo objectinfo_new();
ObjectInfo objectinfo_init(ObjectInfo oi, Loader l, TCOD_list_t dis, MoveInfo mi);
void objectinfo_free(ObjectInfo oi);
void objectinfo_add_drawinfo(ObjectInfo oi, DrawInfo di);
TCOD_list_t objectinfo_drawinfos(ObjectInfo oi);
void objectinfo_set_moveinfo(ObjectInfo oi, MoveInfo mi);
MoveInfo objectinfo_moveinfo(ObjectInfo oi);
bool objectinfo_move_default_allowed(ObjectInfo oi);

bool objectinfo_status_active(ObjectInfo oi, Status s);
void objectinfo_apply_status_named(ObjectInfo oi, char *n);
void objectinfo_revoke_status_named(ObjectInfo oi, char *n);
void objectinfo_apply_status(ObjectInfo oi, Status s);
void objectinfo_revoke_status(ObjectInfo oi, Status s);

void objectinfo_remake_net_moveinfo(ObjectInfo oi);

#endif