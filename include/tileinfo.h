#ifndef _TILE_INFO_H
#define _TILE_INFO_H

struct _tile_info {
  TCOD_list_t drawinfos;
  TCOD_list_t moveinfos;
  bool moveDefaultAllowed;
};

typedef struct _tile_info * TileInfo;

TileInfo tileinfo_new();
TileInfo tileinfo_init(TileInfo ti, TCOD_list_t dis, TCOD_list_t mis, bool moveDefaultAllow);
void tileinfo_free(TileInfo ti);
void tileinfo_add_drawinfo(TileInfo ti, DrawInfo di);
TCOD_list_t tileinfo_drawinfos(TileInfo ti);
void tileinfo_add_moveinfo(TileInfo ti, MoveInfo mi);
TCOD_list_t tileinfo_moveinfos(TileInfo ti);
bool tileinfo_move_default_allowed(TileInfo ti);

#endif