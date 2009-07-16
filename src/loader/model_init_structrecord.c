#include "loader/model_init_structrecord.h"

#include <string.h>

Grant grant_init_structrecord(Grant g, StructRecord sr) {
  char *statusName = structrecord_name(sr);
  float duration = structrecord_has_prop(sr, "duration") ? structrecord_get_prop_value(sr, "duration").f : 0;
  int priority = structrecord_has_prop(sr, "priority") ? structrecord_get_prop_value(sr, "priority").i : 1;
  char *reason = structrecord_has_prop(sr, "reason") ? structrecord_get_prop_value(sr, "reason").s : NULL;
  return grant_init(g, statusName, duration, priority, reason);
}

Revoke revoke_init_structrecord(Revoke r, StructRecord sr) {
  char *statusName = structrecord_name(sr);
  int priority = structrecord_has_prop(sr, "priority") ? structrecord_get_prop_value(sr, "priority").i : 1;
  char *reason = structrecord_has_prop(sr, "reason") ? structrecord_get_prop_value(sr, "reason").s : NULL;
  return revoke_init(r, statusName, priority, reason);
}

EffectSet effect_set_init_structrecord(EffectSet es, StructRecord sr, char *defaultTarget) {
  //srcO, srcV, dstO, dstV
  //set -- get first appropriate flag or property and use it.
  //if property, provide rval; otherwise, provide null.
  //properties override flags.
  //really, you should only have ONE property OR flag in a set. later, this should throw a warning.
  char *dstV = structrecord_name(sr);
  char *dstO = structrecord_has_prop(sr, "target") ? structrecord_get_prop_value(sr, "target").s : defaultTarget;
  char *srcV = structrecord_has_prop(sr, "source") ? structrecord_get_prop_value(sr, "source").s : dstV;
  char *srcO = structrecord_has_prop(sr, "source_object") ? structrecord_get_prop_value(sr, "source_object").s : dstO;
  
  TCOD_list_t flags = structrecord_flags(sr);
  TCOD_list_t props = structrecord_props(sr);
  
  TCOD_value_t *valPtr = NULL;
  SetMode mode = SetNone;
  TS_LIST_FOREACH(flags, 
    mode = effect_set_mode_from_name(each);
    if(mode != ComparisonNone) {
      break;
    }
  );
  TCOD_value_t val;
  TS_LIST_FOREACH(props,
    mode = effect_set_mode_from_name(prop_name(each));
    if(mode != ComparisonNone) {
      val = prop_value(each);
      valPtr = &val;
      break;
    }
  );
  return effect_set_init(es, mode, dstO, dstV, srcO, srcV, valPtr);
}

EffectGrantRevoke effect_grantrevoke_init_structrecord(EffectGrantRevoke gr, StructRecord sr, char *defaultTarget) {
  char *target = structrecord_has_prop(sr, "target") ? structrecord_get_prop_value(sr, "target").s : defaultTarget;
  void *effect = NULL;
  if(STREQ(structrecord_type(sr), "grant")) {
    effect = grant_init_structrecord(grant_new(), sr);
  } else if(STREQ(structrecord_type(sr), "revoke")) {
    effect = revoke_init_structrecord(revoke_new(), sr);
  }
  return effect_grantrevoke_init(gr, effect, target);
}

Check check_init_structrecord(Check c, StructRecord sr, char *defaultTarget) {
  //srcO, srcV, targetO, targetV
  //comparisons -- check for every flag and property.
  //if property, provide rval; otherwise, provide null.
  char *targV = structrecord_name(sr);
  char *targO = structrecord_has_prop(sr, "target") ? structrecord_get_prop_value(sr, "target").s : defaultTarget;
  char *srcV = structrecord_has_prop(sr, "source") ? structrecord_get_prop_value(sr, "source").s : targV;
  char *srcO = structrecord_has_prop(sr, "source_object") ? structrecord_get_prop_value(sr, "source_object").s : targO;
  
  TCOD_list_t comparisons = TCOD_list_new();
  
  TCOD_list_t props = structrecord_props(sr);
  ComparisonMode comparisonMode=ComparisonNone;
  TCOD_value_t val;
  TS_LIST_FOREACH(props, 
    val = prop_value(each);
    comparisonMode = comparison_mode_from_name(prop_name(each));
    if(comparisonMode != ComparisonNone) {
      TCOD_list_push(comparisons, comparison_init(comparison_new(), comparisonMode, &val));
    }
  );
  TCOD_list_t flags = structrecord_flags(sr);
  TS_LIST_FOREACH(flags, 
    comparisonMode = comparison_mode_from_name(each);
    if(comparisonMode != ComparisonNone) {
      TCOD_list_push(comparisons, comparison_init(comparison_new(), comparisonMode, NULL));
    }
  );
  return check_init(c, targV, targO, srcV, srcO, comparisons);
}

Condition condition_init_structrecord(Condition c, StructRecord sr, char *defaultTarget) {
  //all/any, negate, conditions, checks
  TCOD_list_t checks = TCOD_list_new();
  TCOD_list_t subconditions = TCOD_list_new();
  for(int i = 0; i < TCOD_list_size(structrecord_children(sr)); i++) {
    StructRecord kid = TCOD_list_get(structrecord_children(sr), i);
    char *kidType = structrecord_type(kid);
    if(strcmp(kidType, "check") == 0) {
      TCOD_list_push(checks, check_init_structrecord(check_new(), kid, defaultTarget));
    } else if(strcmp(kidType, "condition") == 0) {
      TCOD_list_push(subconditions, condition_init_structrecord(condition_new(), kid, defaultTarget));
    }
  }
  BooleanMode mode = BooleanAll;
  if(structrecord_has_prop(sr, "requires")) {
    mode = (strcmp(
      structrecord_get_prop_value(sr, "requires").s, 
      "all"
    ) == 0) ? BooleanAll : BooleanAny;
  }
  bool negate = structrecord_is_flag_set(sr, "negate");
  return condition_init(c, mode, negate, checks, subconditions);
}

Action action_init_structrecord(Action a, StructRecord sr, FlagSchema trigSchema, char *defaultTarget) {
  TCOD_list_t conditions = TCOD_list_new();
  TCOD_list_t grants = TCOD_list_new();
  TCOD_list_t revokes = TCOD_list_new();
  TCOD_list_t sets = TCOD_list_new();
  Flagset triggers = flagset_init(flagset_new(trigSchema), trigSchema);
  for(int i = 0; i < TCOD_list_size(structrecord_children(sr)); i++) {
    StructRecord kid = TCOD_list_get(structrecord_children(sr), i);
    char *kidType = structrecord_type(kid);
    if(strcmp(kidType, "condition") == 0) {
      TCOD_list_push(conditions, condition_init_structrecord(condition_new(), kid, defaultTarget));
    } else if(strcmp(kidType, "grant") == 0) {
      TCOD_list_push(grants, effect_grantrevoke_init_structrecord(effect_grantrevoke_new(), kid, defaultTarget));
    } else if(strcmp(kidType, "revoke") == 0) {
      TCOD_list_push(revokes, effect_grantrevoke_init_structrecord(effect_grantrevoke_new(), kid, defaultTarget));
    } else if(strcmp(kidType, "set") == 0) {
      TCOD_list_push(sets, effect_set_init_structrecord(effect_set_new(), kid, defaultTarget));
    }
  }
  return action_init(a, structrecord_name(sr), triggers, trigSchema, conditions, grants, revokes, sets);
}

DrawInfo drawinfo_init_structrecord(DrawInfo di, StructRecord sr, int index, int *finalZ) {
  int z = structrecord_get_prop_value_default(sr, "z", (TCOD_value_t)(index)).i;
  TCOD_color_t fore = structrecord_get_prop_value_default(sr, "fore", (TCOD_value_t)((TCOD_color_t){255, 255, 255})).col;
  TCOD_color_t back = structrecord_get_prop_value_default(sr, "back", (TCOD_value_t)((TCOD_color_t){0, 0, 0})).col;
  unsigned char symbol = structrecord_get_prop_value_default(sr, "symbol", (TCOD_value_t)' ').c;
  if(finalZ) {
    *finalZ = z;
  }
  return drawinfo_init(di, z, fore, back, symbol);
}

MoveInfo moveinfo_init_structrecord(MoveInfo mi, StructRecord sr) {
  TCOD_list_t props = structrecord_props(sr);
  TCOD_list_t flags = TCOD_list_new();
  for(int i = 0; i < TCOD_list_size(props); i++) {
    Prop p = TCOD_list_get(props, i);
    TCOD_list_push(flags, moveflag_init(moveflag_new(), prop_name(p), prop_value(p).b));    
  }
  return moveinfo_init(mi, flags);
}

Tile tile_init_structrecord(Tile t, Loader l, StructRecord sr, FlagSchema actionTriggers) {
  TCOD_list_t drawInfos = TCOD_list_new();
  TCOD_list_t moveInfos = TCOD_list_new();
  TCOD_list_t actions = TCOD_list_new();
  int drawIndex = 0;
  for(int i = 0; i < TCOD_list_size(structrecord_children(sr)); i++) {
    StructRecord kid = TCOD_list_get(structrecord_children(sr), i);
    char *kidType = structrecord_type(kid);
    if(strcmp(kidType, "draw") == 0) {
      TCOD_list_push(drawInfos, drawinfo_init_structrecord(drawinfo_new(), kid, drawIndex, &drawIndex));
      drawIndex++;
    } else if(strcmp(kidType, "movement") == 0) {
      TCOD_list_push(moveInfos, moveinfo_init_structrecord(moveinfo_new(), kid));
    } else if(strcmp(kidType, "action") == 0) {
      TCOD_list_push(actions, action_init_structrecord(action_new(), kid, actionTriggers, "object"));
    }
  }
  bool moveDefaultAllowed = true;
  if(structrecord_has_prop(sr, "movement_default")) {
    moveDefaultAllowed = strcmp(
      structrecord_get_prop_value(sr, "movement_default").s, 
      "allow"
    ) == 0;
  }
  bool wallTransp=true, floorTransp=false, ceilTransp=true;
  #warning later, see if we can bring back fractional opacity.
  //it would be a postprocessing step after fov check.
  //use base first
  if(structrecord_has_prop(sr, "opacity")) {
    TCOD_list_t opacityFields = structrecord_get_prop_value(sr, "opacity").list;
    wallTransp = !((unsigned int)TCOD_list_get(opacityFields, 0));
    floorTransp = !((unsigned int)TCOD_list_get(opacityFields, 1));
    ceilTransp = !((unsigned int)TCOD_list_get(opacityFields, 2));
  }
  //then use any shorthands
  if(structrecord_has_prop(sr, "uniform_opacity")) {
    char uniformOpacity = structrecord_get_prop_value(sr, "uniform_opacity").c;
    wallTransp = floorTransp = ceilTransp = !uniformOpacity;
  }
  if(structrecord_has_prop(sr, "wall_opacity")) {
    char wallOpacity = structrecord_get_prop_value(sr, "wall_opacity").c;
    wallTransp = !wallOpacity;
  }
  if(structrecord_has_prop(sr, "floor_opacity")) {
    char floorOpacity = structrecord_get_prop_value(sr, "floor_opacity").c;
    floorTransp = !floorOpacity;
  }
  if(structrecord_has_prop(sr, "ceiling_opacity")) {
    char ceilingOpacity = structrecord_get_prop_value(sr, "ceiling_opacity").c;
    ceilTransp = !ceilingOpacity;
  }
  #warning moveinfo categories for up/down should exist
  //the default, however, should just be to treat the flag 'stairs' as meaning
  //bidirectional and automatic when entered.
  //for now, we'll just use a boolean for 'stairs'
  bool stairs = false;
  if(structrecord_is_flag_set(sr, "stairs")) {
    stairs = true;
  }
  bool pit = false;
  if(structrecord_is_flag_set(sr, "pit")) {
    pit = true;
  }
  #warning descs are being ignored
  TileInfo ti = tileinfo_init(tileinfo_new(), l, actions, drawInfos, moveInfos, moveDefaultAllowed, stairs, pit);
  return tile_init(t, wallTransp, floorTransp, ceilTransp, ti);
}

Map map_init_structrecord(Map m, StructRecord sr) {
  //dimensions, ambient light, tilemap
  //implement these search methods
  int ambientLight = structrecord_get_prop_value(sr, "ambient_light").i;
  TCOD_list_t dimensions = structrecord_get_prop_value(sr, "dimensions").list;
  //make mapSize from floats in dimensions
  int sx = (int)TCOD_list_get(dimensions, 0);
  int sy = (int)TCOD_list_get(dimensions, 1);
  int sz = (int)TCOD_list_get(dimensions, 2);
  mapVec mapSize = (mapVec){sx, sy, sz};
  TCOD_list_t tileIndexList = structrecord_get_prop_value(sr, "tilemap").list;
  //make tileMap from the TCOD_list_t in "tilemap"
  unsigned char *tileMap = calloc(TCOD_list_size(tileIndexList), sizeof(unsigned short));
  for(int i = 0; i < TCOD_list_size(tileIndexList); i++) {
    void *tileID = TCOD_list_get(tileIndexList, i);
    tileMap[i] = *(unsigned char*)(&tileID);
  }
  //make a mapCtx that stores other info.  guess it can be null for now
  void *mapCtx = NULL;
  //also, don't know what to do just yet about null-tile context. null for now!
  void *blankTileCtx = NULL;
  return map_init(map_new(), structrecord_name(sr), mapSize, tileMap, ambientLight, blankTileCtx, mapCtx);
}
