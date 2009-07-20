#include "tileinfo.h"
#include "stdlib.h"

TileInfo tileinfo_new() {
  return calloc(1, sizeof(struct _tile_info));
}
TileInfo tileinfo_init(TileInfo ti, Loader l, TCOD_list_t actions, TCOD_list_t dis, TCOD_list_t mis, bool moveDefaultAllowed, bool stairs, bool pit) {
  ti->loader = l;
  ti->actions = actions ? actions : TCOD_list_new();
  ti->drawinfos = dis ? dis : TCOD_list_new();
  ti->moveinfos = mis ? mis : TCOD_list_new();
  ti->moveDefaultAllowed=moveDefaultAllowed;
  ti->stairs = stairs;
  ti->pit = pit;
  #warning descriptions need to go here!
  return ti;
}
void tileinfo_free(TileInfo ti) {
  if(!ti) { return; }
  TS_LIST_CLEAR_AND_DELETE(ti->actions, action);
  TS_LIST_CLEAR_AND_DELETE(ti->drawinfos, drawinfo);
  TS_LIST_CLEAR_AND_DELETE(ti->moveinfos, moveinfo);
  free(ti);
}
void tileinfo_add_action(TileInfo ti, Action a) {
  if(!ti) { return; }
  TCOD_list_push(ti->actions, a);
}
TCOD_list_t tileinfo_actions(TileInfo ti) {
  return ti ? ti->actions : NULL;
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
  if(!ti) { return true; }
  return ti->moveDefaultAllowed;
}
bool tileinfo_is_stairs(TileInfo ti) {
  return ti ? ti->stairs : false;
}
bool tileinfo_is_pit(TileInfo ti) {
  return ti ? ti->pit : true;
}

bool tileinfo_moveinfo_can_enter(TileInfo ti, MoveInfo mi) {
  if(!ti) { return true; }
  //see if the object's moveinfo matches any of the tile's movementinfos.
  //if so, 
    //if the tile defaults to allow, then deny
    //if the tile defaults to deny, then allow
  bool match = false;
  for(int i = 0; i < TCOD_list_size(ti->moveinfos); i++) {
    MoveInfo tmi = TCOD_list_get(ti->moveinfos, i);
    if(moveinfo_match(tmi, mi)) {
      match = true;
    }
  }
  //XOR -- 
  //match  mda  allow
  //    0    0      0
  //    1    0      1
  //    0    1      1
  //    1    1      0
  //if match is true and move_default_allowed is true, we deny; if match is true and; if match is false and move_default_allowed is false, we allow;
  //This is what logical xor looks like in C.  Sure would be nice to have ^^!
  return (!match != !(ti->moveDefaultAllowed));
}

void tileinfo_trigger(TileInfo ti, Tile self, Object walker, char *trig) {
  if(!ti) { return; }
  Flagset trigger = loader_make_trigger(ti->loader, trig);
  Bindings defaultBindings = bindings_init(bindings_new(), NULL, "root", "root", NULL);
  bindings_insert(defaultBindings, "tile", self);
  bindings_insert(defaultBindings, "object", walker);
  //DUNNO LOL, TRIGGER SOME ACTIONS
  TS_LIST_FOREACH(ti->actions,
    action_bind(each, defaultBindings);
    action_apply(each, trigger);
  );
  bindings_free(defaultBindings);
  flagset_free(trigger);
}