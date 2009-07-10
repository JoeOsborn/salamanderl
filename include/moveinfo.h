#ifndef _MOVE_INFO_H
#define _MOVE_INFO_H

#include <tilesense.h>

struct _move_info {
  TCOD_list_t flags;
};

typedef struct _move_info * MoveInfo;


struct _move_flag {
  char *label;
  bool value;
};

typedef struct _move_flag *MoveFlag;


MoveInfo moveinfo_new();
MoveInfo moveinfo_init(MoveInfo mi, TCOD_list_t flags);
void moveinfo_free(MoveInfo mi);
TCOD_list_t moveinfo_flags(MoveInfo mi);
bool moveinfo_match(MoveInfo m1, MoveInfo m2);

MoveFlag moveflag_new();
MoveFlag moveflag_init(MoveFlag mf, char *label, bool value);
char *moveflag_label(MoveFlag mf);
bool moveflag_value(MoveFlag mf);
bool moveflag_matches(MoveFlag mf, MoveInfo mi);
void moveflag_free(MoveFlag mf);

#endif