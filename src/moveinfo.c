#include "moveinfo.h"
#include "stdlib.h"

MoveFlag moveflag_new() {
  return calloc(1, sizeof(struct _move_flag));
}

MoveFlag moveflag_init(MoveFlag mf, char *label, bool value) {
  mf->label = strdup(label);
  mf->value = value;
  return mf;
}

char *moveflag_label(MoveFlag mf) {
  return mf->label;
}

bool moveflag_value(MoveFlag mf) {
  return mf->value;
}

bool moveflag_matches(MoveFlag mf, MoveInfo mi) {
  TCOD_list_t otherFlags = moveinfo_flags(mi);
  MoveFlag otherMF;
  for(int i = 0; i < TCOD_list_size(otherFlags); i++) {
    if(strcmp(mf->label, moveflag_label(otherMF)) == 0) {
      return mf->value == otherMF->value;
    }
  }
  return false;
}

void moveflag_free(MoveFlag mf) {
  free(mf->label);
  free(mf);
}

MoveInfo moveinfo_new() {
  return calloc(1, sizeof(struct _move_info));
}
MoveInfo moveinfo_init(MoveInfo mi, TCOD_list_t flags) {
  mi->flags = flags;
  return mi;
}
void moveinfo_free(MoveInfo mi) {
  for(int i = 0; i < TCOD_list_size(mi->flags); i++) {
    moveflag_free(TCOD_list_get(mi->flags, i));
  }
  TCOD_list_delete(mi->flags);
  free(mi);
}
TCOD_list_t moveinfo_flags(MoveInfo mi) {
  if(!mi) { return NULL; }
  return mi->flags;
}
bool moveinfo_match(MoveInfo m1, MoveInfo m2) {
  if(!m1) { return true; }
  if(!m2) { return false; }
  //for each flag defined in m1, see if m2 defines it the same way.
  for(int i = 0; i < TCOD_list_size(m1->flags); i++) {
    MoveFlag mf = TCOD_list_get(m1->flags, i);
    if(!moveflag_matches(mf, m2)) {
      return false;
    }
  }
  return true;
}