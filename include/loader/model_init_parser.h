#ifndef _MODEL_INIT_PRS
#define _MODEL_INIT_PRS

#include <tilesense.h>
#include <libtcod.h>

#include "drawinfo.h"
#include "moveinfo.h"
#include "tileinfo.h"
#include "objectinfo.h"
#include "status.h"

#include "action/action.h"
#include "action/bindings.h"
#include "action/check.h"
#include "action/comparison.h"
#include "action/condition.h"
#include "action/effect_grantrevoke.h"
#include "action/effect_message.h"
#include "action/effect_set.h"

TCOD_parser_struct_t grant_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t revoke_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t status_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t effect_set_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t check_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t condition_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t action_init_parser(TCOD_parser_t p, Loader l);

TCOD_parser_struct_t drawinfo_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t moveinfo_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t tile_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t map_init_parser(TCOD_parser_t p, Loader l);
TCOD_parser_struct_t object_init_parser(TCOD_parser_t p, Loader l);

void triggerdesc_init_parser(TCOD_parser_t p, TCOD_parser_struct_t st, Loader l);

#endif