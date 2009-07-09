#include "moveinfo.h"
#include "stdlib.h"

MoveInfo moveinfo_new() {
  return calloc(1, sizeof(struct _move_info));
}
MoveInfo moveinfo_init(MoveInfo mi, FlagSchema sch, Flagset flags) {
  mi->schema = sch;
  mi->flags = flags;
  return mi;
}
void moveinfo_free(MoveInfo mi) {
  flagschema_free(mi->schema);
  flagset_free(mi->flags);
  free(mi);
}