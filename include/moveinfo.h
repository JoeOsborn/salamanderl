#ifndef _MOVE_INFO_H
#define _MOVE_INFO_H

#include <tilesense.h>

struct _move_info {
  FlagSchema schema;
  Flagset flags;
};

typedef struct _move_info * MoveInfo;

MoveInfo moveinfo_new();
MoveInfo moveinfo_init(MoveInfo mi, FlagSchema sch, Flagset flags);
void moveinfo_free(MoveInfo mi);

#endif