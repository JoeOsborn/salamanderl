#ifndef _OBJECT_INFO_H
#define _OBJECT_INFO_H

#include <libtcod.h>
#include "drawinfo.h"
#include "moveinfo.h"

struct _object_info {
  TCOD_list_t drawinfos;
  MoveInfo moveinfo;
};

typedef struct _object_info *ObjectInfo;

ObjectInfo objectinfo_new();
ObjectInfo objectinfo_init(ObjectInfo oi, TCOD_list_t dis, MoveInfo mi);
void objectinfo_free(ObjectInfo oi);
void objectinfo_add_drawinfo(ObjectInfo oi, DrawInfo di);
TCOD_list_t objectinfo_drawinfos(ObjectInfo oi);
void objectinfo_set_moveinfo(ObjectInfo oi, MoveInfo mi);
MoveInfo objectinfo_moveinfo(ObjectInfo oi);
bool objectinfo_move_default_allowed(ObjectInfo oi);

#endif