#ifndef _MODEL_INIT_SR
#define _MODEL_INIT_SR

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
#include "action/effect_change_map.h"
#include "action/effect_place_object.h"

Grant grant_init_structrecord(Grant g, StructRecord sr);
Revoke revoke_init_structrecord(Revoke r, StructRecord sr);
Status status_init_structrecord(Status s, StructRecord sr);

EffectSet effect_set_init_structrecord(EffectSet es, StructRecord sr, char *defaultTarget);
EffectGrantRevoke effect_grantrevoke_init_structrecord(EffectGrantRevoke gr, StructRecord sr, char *defaultTarget);

Check check_init_structrecord(Check c, StructRecord sr, char *defaultTarget);
Condition condition_init_structrecord(Condition c, StructRecord sr, char *defaultTarget);

Action action_init_structrecord(Action a, StructRecord sr, FlagSchema trigSchema, char *defaultTarget);


DrawInfo drawinfo_init_structrecord(DrawInfo di, StructRecord sr, int index, int *finalZ);

MoveInfo moveinfo_init_structrecord(MoveInfo mi, StructRecord sr);

Tile tile_init_structrecord(Tile t, Loader l, StructRecord sr, FlagSchema actionTriggers);

Map map_init_structrecord(Map m, StructRecord sr);

Object object_init_structrecord_overrides(Object o, Loader l, StructRecord sr, StructRecord over);

mapVec mapvec_make_int_list(TCOD_list_t l);
mapVec mapvec_make_float_list(TCOD_list_t l);

#endif