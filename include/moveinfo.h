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
void moveinfo_add_flag(MoveInfo mi, MoveFlag mf);
bool moveinfo_match(MoveInfo m1, MoveInfo m2);
void moveinfo_combine(MoveInfo m1, MoveInfo m2);
void moveinfo_combine_list(MoveInfo mi, TCOD_list_t moveInfos);
MoveFlag moveinfo_get_flag(MoveInfo m, char *flag);
bool moveinfo_flag_value(MoveInfo m, char *flag);
void moveinfo_set_flag(MoveInfo m, char *flag, bool value);

MoveFlag moveflag_new();
MoveFlag moveflag_init(MoveFlag mf, char *label, bool value);
MoveFlag moveflag_copy(MoveFlag mf1, MoveFlag mf2);
void moveflag_free(MoveFlag mf);
char *moveflag_label(MoveFlag mf);
bool moveflag_value(MoveFlag mf);
void moveflag_set_value(MoveFlag mf, bool value);
bool moveflag_matches(MoveFlag mf, MoveInfo mi);
#endif