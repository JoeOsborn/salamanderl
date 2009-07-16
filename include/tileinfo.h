#ifndef _TILE_INFO_H
#define _TILE_INFO_H

#include <libtcod.h>
#include "drawinfo.h"
#include "moveinfo.h"
#include "action/action.h"
#include "loader.h"

struct _tile_info {
  Loader loader;
  
  TCOD_list_t actions;
  TCOD_list_t drawinfos;
  TCOD_list_t moveinfos;
  bool moveDefaultAllowed;
  bool stairs, pit;
};

typedef struct _tile_info * TileInfo;

TileInfo tileinfo_new();
TileInfo tileinfo_init(TileInfo ti, Loader l, TCOD_list_t actions, TCOD_list_t dis, TCOD_list_t mis, bool moveDefaultAllow, bool stairs, bool pit);
void tileinfo_free(TileInfo ti);
void tileinfo_add_action(TileInfo ti, Action a);
TCOD_list_t tileinfo_actions(TileInfo ti);
void tileinfo_add_drawinfo(TileInfo ti, DrawInfo di);
TCOD_list_t tileinfo_drawinfos(TileInfo ti);
void tileinfo_add_moveinfo(TileInfo ti, MoveInfo mi);
TCOD_list_t tileinfo_moveinfos(TileInfo ti);
bool tileinfo_move_default_allowed(TileInfo ti);
bool tileinfo_is_stairs(TileInfo ti);
bool tileinfo_is_pit(TileInfo ti);

bool tileinfo_moveinfo_can_enter(TileInfo ti, MoveInfo mi);

void tileinfo_trigger(TileInfo ti, Object walker, char *trig);

#endif