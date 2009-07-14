#include "status.h"
#include <stdlib.h>
#include <string.h>

#include "moveinfo.h"
#include "loader.h"

Grant grant_new() {
  return calloc(1, sizeof(struct _grant));
}
Grant grant_init(Grant g, char *n, float d, int p, void *cause) {
  g->status = strdup(n);
  g->duration = d;
  g->priority = p;
  g->cause = cause;
  return g;
}
void grant_free(Grant g) {
  free(g->status);
  free(g);
}
char *grant_status(Grant g) {
  return g->status;
}
float grant_duration(Grant g) {
  return g->duration;
}
int grant_priority(Grant g) {
  return g->priority;
}
void *grant_cause(Grant g) {
  return g->cause;
}
bool grant_active(Grant g) {
  return g->active;
}
void grant_set_active(Grant g, bool active) {
  g->active = active;
}


Revoke revoke_new() {
  return calloc(1, sizeof(struct _revoke));
}
Revoke revoke_init(Revoke r, char *n, int p, void *cause) {
  r->status = strdup(n);
  r->priority = p;
  r->cause = cause;
  return r;
}
void revoke_free(Revoke r) {
  free(r->status);
  free(r);
}
char *revoke_status(Revoke r) {
  return r->status;
}
int revoke_priority(Revoke r) {
  return r->priority;
}
void *revoke_cause(Revoke r) {
  return r->cause;
}

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
  if(s->activateMsg) { free(s->activateMsg); }
  if(s->onDescription) { free(s->onDescription); }
  TS_LIST_CLEAR_AND_DELETE(s->onRevokes, revoke);
  TS_LIST_CLEAR_AND_DELETE(s->onGrants, grant);
  TS_LIST_CLEAR_AND_DELETE(s->onMoves, moveinfo);
  if(s->deactivateMsg) { free(s->deactivateMsg); }
  TS_LIST_CLEAR_AND_DELETE(s->offRevokes, revoke);
  TS_LIST_CLEAR_AND_DELETE(s->offGrants, grant);
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
