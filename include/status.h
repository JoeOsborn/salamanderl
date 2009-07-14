#ifndef _STATUS_H
#define _STATUS_H

#include <libtcod.h>

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