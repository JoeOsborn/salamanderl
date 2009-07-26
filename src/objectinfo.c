#include "objectinfo.h"
#include "stdlib.h"
#include "action/action.h"
#include "action/bindings.h"

ObjectInfo objectinfo_new() {
  return calloc(1, sizeof(struct _object_info));
}
ObjectInfo objectinfo_init(ObjectInfo oi, Loader l, TCOD_list_t dis, MoveInfo mi, TCOD_list_t actions, int weight, char *desc) {
  oi->loader = l;
  oi->drawinfos = dis ? dis : TCOD_list_new();
  oi->actions = actions ? actions : TCOD_list_new();
  oi->grants = TCOD_list_new();
  oi->revokes = TCOD_list_new();
  oi->statuses = TCOD_list_new();
  oi->moveinfos = TCOD_list_new();
  TCOD_list_push(oi->moveinfos, mi);
  oi->weight = weight;
  oi->stomach = TCOD_list_new();
  oi->attachedObject = NULL;
  oi->attachMode = AttachNone;
  oi->description = desc ? strdup(desc) : NULL;
  objectinfo_remake_net_moveinfo(oi);
  return oi;
}
void objectinfo_free(ObjectInfo oi) {
  if(!oi) { return; }
  TS_LIST_CLEAR_AND_DELETE(oi->statuses, status);
  TS_LIST_CLEAR_AND_DELETE(oi->drawinfos, drawinfo);
  TS_LIST_CLEAR_AND_DELETE(oi->actions, action);
//  TS_LIST_CLEAR_AND_DELETE(oi->stomach, food);
  moveinfo_free(TCOD_list_get(oi->moveinfos, 0));
  TCOD_list_delete(oi->moveinfos);
  moveinfo_free(oi->netMoveinfo);
  free(oi->description);
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

void objectinfo_grant(ObjectInfo oi, Grant g) {
  char *statName = grant_status(g);
  Status s = loader_get_status(oi->loader, statName);
  TCOD_list_push(oi->grants, g);
  grant_set_active(g, true);
  //later, if necessary, do a check on the 'severity' of the status?
  //or can that stay in grant?  save it for a future project... this is
  //probably all unnecessary.
  if(!objectinfo_status_active(oi, s)) {
    objectinfo_apply_status(oi, s);
  }
}
void objectinfo_revoke(ObjectInfo oi, Revoke r) {
  char *statName = revoke_status(r);
  Status s = loader_get_status(oi->loader, statName);
  TCOD_list_push(oi->revokes, r);
  if(!objectinfo_status_active(oi, s)) {
    return;
  }
  for(int i = 0; i < TCOD_list_size(oi->grants); i++) {
    Grant g = TCOD_list_get(oi->grants, i);
    if(grant_active(g)) {
      if(grant_priority(g) <= revoke_priority(r)) {
        Status gs = loader_get_status(oi->loader, grant_status(g));
        if(s == gs) {
          grant_set_active(g, false);
          objectinfo_revoke_status(oi, s);
        }
      }
    }
  }
}

void objectinfo_apply_status(ObjectInfo oi, Status s) {
  //it may be that revoking before granting provides better behavior in loops.
  
  TCOD_list_t revokes = status_onrevokes(s);
  for(int i = 0; i < TCOD_list_size(revokes); i++) {
    Revoke r = TCOD_list_get(revokes, i);
    objectinfo_revoke(oi, r);
  }
  
  TCOD_list_push(oi->statuses, s);
  TCOD_list_add_all(oi->moveinfos, status_onmoveinfos(s));
  moveinfo_combine_list(oi->netMoveinfo, status_onmoveinfos(s));
  
  TCOD_list_t grants = status_ongrants(s);
  for(int i = 0; i < TCOD_list_size(grants); i++) {
    Grant g = TCOD_list_get(grants, i);
    objectinfo_grant(oi, g);
  }
}

void objectinfo_revoke_status(ObjectInfo oi, Status s) {
  TCOD_list_t revokes = status_offrevokes(s);
  for(int i = 0; i < TCOD_list_size(revokes); i++) {
    Revoke r = TCOD_list_get(revokes, i);
    objectinfo_revoke(oi, r);
  }

  TCOD_list_remove(oi->statuses, s);
  TCOD_list_remove_all(oi->moveinfos, status_onmoveinfos(s));
  //what about persistent effects?  Just leave them dangling?
  //or should there be a separate, objectinfo-owned area for them?
  objectinfo_remake_net_moveinfo(oi);

  TCOD_list_t grants = status_offgrants(s);
  for(int i = 0; i < TCOD_list_size(grants); i++) {
    Grant g = TCOD_list_get(grants, i);
    objectinfo_grant(oi, g);
  }
}

void objectinfo_remake_net_moveinfo(ObjectInfo oi) {
  if(oi->netMoveinfo) {
    moveinfo_free(oi->netMoveinfo);
  }
  oi->netMoveinfo = moveinfo_init(moveinfo_new(), NULL);
  moveinfo_combine_list(oi->netMoveinfo, oi->moveinfos);
}

char *objectinfo_description(ObjectInfo oi) {
  return oi ? oi->description : NULL;
}

void objectinfo_eat(ObjectInfo oi, Object food, float volume, float digestTime) {
  #warning stomach stuff
  objectinfo_set_chomping(oi, true);
  objectinfo_set_chomping(oi, false);
}

Object objectinfo_attached_object(ObjectInfo oi) {
  return oi->attachedObject;
}
AttachMode objectinfo_attach_mode(ObjectInfo oi) {
  return oi->attachMode;
}
//should attached objects be a list, or just a single object?
void objectinfo_attach(ObjectInfo oi, Object o, Object o2, AttachMode mode) {
  if(!oi->chomping) {
    oi->chomping = true;
    oi->attachedObject = o2;
    oi->attachMode = mode;
    objectinfo_trigger(object_context(o2), o2, o, "on_attach");
  }
}
void objectinfo_detach(ObjectInfo oi, Object o, Object o2) {
  if(oi->attachedObject == o2) {
    oi->attachedObject = NULL;
    oi->attachMode = AttachNone;
    oi->chomping = false;
    objectinfo_trigger(object_context(o2), o2, o, "on_release");
  }
}

bool objectinfo_trigger(ObjectInfo oi, Object self, Object other, char *trig) {
  if(!oi) { return false; }
  Flagset trigger = loader_make_trigger(oi->loader, trig);
  Bindings defaultBindings = bindings_init(bindings_new(), NULL, "root", "root", NULL);
  bindings_insert(defaultBindings, "loader", oi->loader);
  bindings_insert(defaultBindings, "self", self);
  bindings_insert(defaultBindings, "map", object_map(self));
  bindings_insert(defaultBindings, "other", other);
  bool result = false;
  TS_LIST_FOREACH(oi->actions,
    //in case the map gets changed on us
    bindings_set_value_path(defaultBindings, "map", object_map(self));
    action_bind(each, defaultBindings);
    if(action_apply(each, trigger)) {
      result = true;
    }
  );
  bindings_free(defaultBindings);
  flagset_free(trigger);
  return result;
}

bool objectinfo_chomping(ObjectInfo oi) {
  return oi->chomping;
}
void objectinfo_set_chomping(ObjectInfo oi, bool c) {
  oi->chomping = c;
}
bool objectinfo_falling(ObjectInfo oi) {
  return oi->falling;
}
void objectinfo_set_falling(ObjectInfo oi, bool f) {
  oi->falling = f;
}
bool objectinfo_underwater(ObjectInfo oi) {
  return oi->underwater;
}
void objectinfo_set_underwater(ObjectInfo oi, bool u) {
  oi->underwater = u;
}

int objectinfo_net_weight(ObjectInfo oi) {
  int w = oi->weight;
  if(oi->attachedObject && oi->attachMode == AttachCarry) {
    ObjectInfo attachedOi = object_context(oi->attachedObject);
    w += attachedOi->weight;
  }
  //add stomach weight
  return w;
}