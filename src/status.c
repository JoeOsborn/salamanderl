#include "status.h"
#include <stdlib.h>
#include <string.h>

#include "moveinfo.h"
#include "loader.h"

Status status_new() {
  return calloc(1, sizeof(struct _status));
}
Status status_init(Status s, char *name, 
  TCOD_list_t onRevokes, 
  TCOD_list_t onGrants, 
  TCOD_list_t onMoves, 
  char *onDesc,
  char *activateMsg,
  
  TCOD_list_t offRevokes, 
  TCOD_list_t offGrants, 
  char *deactivateMsg
) {
  s->onRevokes = onRevokes ? onRevokes : TCOD_list_new();
  s->onGrants = onGrants ? onGrants : TCOD_list_new();
  s->onMoves = onMoves ? onMoves : TCOD_list_new();
  s->onDescription = onDesc ? strdup(onDesc) : NULL;
  s->activateMsg = activateMsg ? strdup(activateMsg) : NULL;

  s->offRevokes = offRevokes ? offRevokes : TCOD_list_new();
  s->offGrants = offGrants ? offGrants : TCOD_list_new();
  s->deactivateMsg = deactivateMsg ? strdup(deactivateMsg) : NULL;

  return s;
}

void status_free(Status s) {
  free(s->activateMsg);
  free(s->onDescription);
  TCOD_list_clear_and_delete(s->onRevokes);
  TCOD_list_clear_and_delete(s->onGrants);
  for(int i = 0; i < TCOD_list_size(s->onMoves); i++) {
    moveinfo_free(TCOD_list_get(s->onMoves, i));
  }
  TCOD_list_delete(s->onMoves);
  free(s->deactivateMsg);
  TCOD_list_clear_and_delete(s->offRevokes);
  TCOD_list_clear_and_delete(s->offGrants);
  free(s);
}

TCOD_list_t status_onrevokes(Status s) {
  return s->onRevokes;
}
TCOD_list_t status_ongrants(Status s) {
  return s->onGrants;
}
TCOD_list_t status_onmoveinfos(Status s) {
  return s->onMoves;
}
TCOD_list_t status_ondescription(Status s) {
  return s->onDescription;
}
TCOD_list_t status_activatemsg(Status s) {
  return s->activateMsg;
}
TCOD_list_t status_offrevokes(Status s) {
  return s->offRevokes;
}
TCOD_list_t status_offgrants(Status s) {
  return s->offGrants;
}
TCOD_list_t status_deactivatemsg(Status s) {
  return s->deactivateMsg;
}
