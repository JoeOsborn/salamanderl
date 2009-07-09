#include "tileinfo.h"
#include "stdlib.h"

TileInfo tileinfo_new() {
  return calloc(1, sizeof(struct _tile_info));
}
TileInfo tileinfo_init(TileInfo ti, TCOD_list_t dis, TCOD_list_t mis, bool moveDefaultAllowed) {
  ti->drawinfos = dis ? dis : TCOD_list_new();
  ti->moveinfos = mis ? mis : TCOD_list_new();
  ti->moveDefaultAllowed=moveDefaultAllowed;
  return ti;
}
void tileinfo_free(TileInfo ti) {
  if(!ti) { return; }
  for(int i = 0; i < TCOD_list_size(ti->drawinfos); i++) {
    drawinfo_free(TCOD_list_get(ti->drawinfos, i));
  }
  TCOD_list_delete(ti->drawinfos);
  for(int i = 0; i < TCOD_list_size(ti->moveinfos); i++) {
    moveinfo_free(TCOD_list_get(ti->moveinfos, i));
  }
  TCOD_list_delete(ti->moveinfos);
  free(ti);
}
void tileinfo_add_drawinfo(TileInfo ti, DrawInfo di) {
  if(!ti) { return; }
  TCOD_list_push(ti->drawinfos, di);
}
TCOD_list_t tileinfo_drawinfos(TileInfo ti) {
  if(!ti) { return NULL; }
  return ti->drawinfos;
}
void tileinfo_add_moveinfo(TileInfo ti, MoveInfo mi) {
  if(!ti) { return; }
  TCOD_list_push(ti->moveinfos, mi);
}
TCOD_list_t tileinfo_moveinfos(TileInfo ti) {
  if(!ti) { return NULL; }
  return ti->moveinfos;
}
bool tileinfo_move_default_allowed(TileInfo ti) {
  return ti->move_default_allowed;
}