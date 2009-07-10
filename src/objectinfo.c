#include "objectinfo.h"
#include "stdlib.h"

ObjectInfo objectinfo_new() {
  return calloc(1, sizeof(struct _object_info));
}
ObjectInfo objectinfo_init(ObjectInfo oi, TCOD_list_t dis, MoveInfo mi) {
  oi->drawinfos = dis ? dis : TCOD_list_new();
  oi->moveinfo = mi;
  return oi;
}
void objectinfo_free(ObjectInfo oi) {
  if(!oi) { return; }
  for(int i = 0; i < TCOD_list_size(oi->drawinfos); i++) {
    drawinfo_free(TCOD_list_get(oi->drawinfos, i));
  }
  TCOD_list_delete(oi->drawinfos);
  moveinfo_free(oi->moveinfo);
  free(oi);
}
void objectinfo_add_drawinfo(ObjectInfo oi, DrawInfo di) {
  if(!oi) { return; }
  TCOD_list_push(oi->drawinfos, di);
}
TCOD_list_t objectinfo_drawinfos(ObjectInfo oi) {
  if(!oi) { return NULL; }
  return oi->drawinfos;
}
void objectinfo_set_moveinfo(ObjectInfo oi, MoveInfo mi) {
  if(!oi) { return; }
  oi->moveinfo = mi;
}
MoveInfo objectinfo_moveinfo(ObjectInfo oi) {
  if(!oi) { return NULL; }
  return oi->moveinfo;
}
