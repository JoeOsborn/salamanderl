#include "objectinfo.h"
#include "stdlib.h"

ObjectInfo objectinfo_new() {
  return calloc(1, sizeof(struct _object_info));
}
ObjectInfo objectinfo_init(ObjectInfo oi, Loader l, TCOD_list_t dis, MoveInfo mi) {
  oi->drawinfos = dis ? dis : TCOD_list_new();
  oi->statuses = TCOD_list_new();
  oi->moveinfos = TCOD_list_new();
  TCOD_list_push(oi->moveinfos, mi);
  objectinfo_remake_net_moveinfo(oi);
  return oi;
}
void objectinfo_free(ObjectInfo oi) {
  if(!oi) { return; }
  for(int i = 0; i < TCOD_list_size(oi->statuses); i++) {
    status_free(TCOD_list_get(oi->statuses, i));
  }
  TCOD_list_delete(oi->statuses);
  for(int i = 0; i < TCOD_list_size(oi->drawinfos); i++) {
    drawinfo_free(TCOD_list_get(oi->drawinfos, i));
  }
  TCOD_list_delete(oi->drawinfos);
  
  moveinfo_free(TCOD_list_get(oi->moveinfos, 0));
  TCOD_list_delete(oi->moveinfos);
  moveinfo_free(oi->netMoveinfo);
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
  MoveInfo oldBase = TCOD_list_get(oi->moveinfos, 0);
  TCOD_list_set(oi->moveinfos, 0, mi);
  moveinfo_free(oldBase);
  objectinfo_remake_net_moveinfo(oi);
}
MoveInfo objectinfo_moveinfo(ObjectInfo oi) {
  if(!oi) { return NULL; }
  return oi->netMoveinfo;
}

bool objectinfo_status_active(ObjectInfo oi, Status s) {
  return TCOD_list_contains(oi->statuses, s);
}

void objectinfo_apply_status_named(ObjectInfo oi, char *n) {
  objectinfo_apply_status(oi, loader_get_status(oi->loader, n));
}
void objectinfo_revoke_status_named(ObjectInfo oi, char *n) {
  objectinfo_revoke_status(oi, loader_get_status(oi->loader, n));
}

void objectinfo_apply_status(ObjectInfo oi, Status s) {
  if(objectinfo_status_active(oi, s)) {
    return;
  }
  //it may be that revoking before granting provides better behavior in loops.
  
  TCOD_list_t revokes = status_onrevokes(s);
  for(int i = 0; i < TCOD_list_size(revokes); i++) {
    Status s = TCOD_list_get(revokes, i);
    objectinfo_revoke_status(oi, s);
  }
  
  TCOD_list_push(oi->statuses, s);
  TCOD_list_add_all(oi->moveinfos, status_onmoveinfos(s));
  moveinfo_combine_list(oi->netMoveinfo, status_onmoveinfos(s));
  
  TCOD_list_t grants = status_ongrants(s);
  for(int i = 0; i < TCOD_list_size(grants); i++) {
    Status s = TCOD_list_get(grants, i);
    objectinfo_apply_status(oi, s);
  }
}

void objectinfo_revoke_status(ObjectInfo oi, Status s) {
  if(!objectinfo_status_active(oi, s)) {
    return;
  }
  TCOD_list_t revokes = status_offrevokes(s);
  for(int i = 0; i < TCOD_list_size(revokes); i++) {
    Status s = TCOD_list_get(revokes, i);
    objectinfo_revoke_status(oi, s);
  }

  TCOD_list_remove(oi->statuses, s);
  TCOD_list_remove_all(oi->moveinfos, status_onmoveinfos(s));
#warning persistent effects
  //what about persistent effects?  Just leave them dangling?
  //or should there be a separate, objectinfo-owned area for them?
  objectinfo_remake_net_moveinfo(oi);

  TCOD_list_t grants = status_offgrants(s);
  for(int i = 0; i < TCOD_list_size(grants); i++) {
    Status s = TCOD_list_get(grants, i);
    objectinfo_apply_status(oi, s);
  }
}

void objectinfo_remake_net_moveinfo(ObjectInfo oi) {
  if(oi->netMoveinfo) {
    moveinfo_free(oi->netMoveinfo);
  }
  oi->netMoveinfo = moveinfo_init(moveinfo_new(), NULL);
  moveinfo_combine_list(oi->netMoveinfo, oi->moveinfos);
}