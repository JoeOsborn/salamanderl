#ifndef _STATUS_H
#define _STATUS_H

#include <libtcod.h>

struct _grant {
  char *status;
  float duration; //seconds
  int priority;
  void *cause;
  
  //should not be used until the grant has been applied.
  bool active;
  float timeLeft;
};
typedef struct _grant *Grant;

Grant grant_new();
Grant grant_init(Grant g, char *n, float d, int p, void *cause);
void grant_free(Grant g);
char *grant_status(Grant g);
float grant_duration(Grant g);
int grant_priority(Grant g);
void *grant_cause(Grant g);

bool grant_active(Grant g);
void grant_set_active(Grant g, bool active);

struct _revoke {
  char *status;
  int priority;
  void *cause;
};
typedef struct _revoke *Revoke;

Revoke revoke_new();
Revoke revoke_init(Revoke r, char *n, int p, void *cause);
void revoke_free(Revoke r);
char *revoke_status(Revoke r);
int revoke_priority(Revoke r);
void *revoke_cause(Revoke r);

struct _status {
  char *name;
  
  TCOD_list_t onRevokes;
  TCOD_list_t onGrants;
  TCOD_list_t onMoves;
  char *onDescription;
  char *activateMsg;

  TCOD_list_t offRevokes;
  TCOD_list_t offGrants;
  char *deactivateMsg;
};
typedef struct _status *Status;

Status status_new();
Status status_init(Status s, char *name, 
  TCOD_list_t onRevokes, 
  TCOD_list_t onGrants, 
  TCOD_list_t onMoves, 
  char *onDescription, 
  char *activateMsg, 
  
  TCOD_list_t offRevokes, 
  TCOD_list_t offGrants, 
  char *deactivateMsg
);
void status_free(Status s);

TCOD_list_t status_onrevokes(Status s);
TCOD_list_t status_ongrants(Status s);
TCOD_list_t status_onmoveinfos(Status s);
TCOD_list_t status_ondescription(Status s);
TCOD_list_t status_activatemsg(Status s);
TCOD_list_t status_offrevokes(Status s);
TCOD_list_t status_offgrants(Status s);
TCOD_list_t status_deactivatemsg(Status s);

#endif