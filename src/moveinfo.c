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

MoveFlag moveflag_copy(MoveFlag mf1, MoveFlag mf2) {
  return moveflag_init(mf1, mf2->label, mf2->value);
}

char *moveflag_label(MoveFlag mf) {
  return mf->label;
}

bool moveflag_value(MoveFlag mf) {
  return mf->value;
}

void moveflag_set_value(MoveFlag mf, bool value) {
  mf->value = value;
}

bool moveflag_matches(MoveFlag mf, MoveInfo mi) {
  TCOD_list_t otherFlags = moveinfo_flags(mi);
  MoveFlag otherMF;
  for(int i = 0; i < TCOD_list_size(otherFlags); i++) {
    otherMF=TCOD_list_get(otherFlags, i);
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
  mi->flags = flags ? flags : TCOD_list_new();
  return mi;
}
void moveinfo_free(MoveInfo mi) {
  TS_LIST_CLEAR_AND_DELETE(mi->flags, moveflag);
  free(mi);
}
TCOD_list_t moveinfo_flags(MoveInfo mi) {
  if(!mi) { return NULL; }
  return mi->flags;
}
void moveinfo_combine(MoveInfo m1, MoveInfo m2) {
  if(!m1 || !m2) { return; }
  for(int i = 0; i < TCOD_list_size(m2->flags); i++) {
    MoveFlag mf = TCOD_list_get(m2->flags, i);
    char *label = moveflag_label(mf);
    moveinfo_set_flag(m1, label, moveinfo_flag_value(m2, label));
  }
}
void moveinfo_combine_list(MoveInfo mi, TCOD_list_t moveInfos) {
  if(!mi || !moveInfos) { return; }
  for(int i = 0; i < TCOD_list_size(moveInfos); i++) {
    moveinfo_combine(mi, TCOD_list_get(moveInfos, i));
  }
}
MoveFlag moveinfo_get_flag(MoveInfo m, char *flag) {
  for(int i = 0; i < TCOD_list_size(m->flags); i++) {
    MoveFlag mf = TCOD_list_get(m->flags, i);
    if(strcmp(moveflag_label(mf), flag) == 0) {
      return mf;
    }
  }
  return NULL;
}
void moveinfo_add_flag(MoveInfo mi, MoveFlag mf) {
  TCOD_list_push(mi->flags, mf);
}
bool moveinfo_flag_value(MoveInfo m, char *flag) {
  MoveFlag mf = moveinfo_get_flag(m, flag);
  return mf ? moveflag_value(mf) : false;
}
void moveinfo_set_flag(MoveInfo m, char *flag, bool value) {
  MoveFlag mf = moveinfo_get_flag(m, flag);
  if(mf) { moveflag_set_value(mf, value); }
  else { moveinfo_add_flag(m, moveflag_init(moveflag_new(), flag, value)); }
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
