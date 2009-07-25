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

Status status_init_structrecord(Status s, StructRecord sr) {
  TCOD_list_t onRevokes = TCOD_list_new();
  TCOD_list_t onGrants = TCOD_list_new();
  TCOD_list_t onMoves = TCOD_list_new();
  
  char *onMsg=NULL;
  char *onDesc=NULL;

  TCOD_list_t offGrants = TCOD_list_new();
  TCOD_list_t offRevokes = TCOD_list_new();
  char *offMsg=NULL;
  
  TCOD_list_t kids = structrecord_children(sr);
  TCOD_list_t subkids = NULL;
  for(int i = 0; i < TCOD_list_size(kids); i++) {
    StructRecord kid = TCOD_list_get(kids, i);
    subkids = structrecord_children(kid);
    if(STREQ(structrecord_type(kid), "on")) {
      //add to onStuff
      TS_LIST_FOREACH(subkids,
        char *n = structrecord_type(each);
        if(STREQ(n, "movement")) {
          TCOD_list_push(onMoves, moveinfo_init_structrecord(moveinfo_new(), each));
        } else if(STREQ(n, "grant")) {
          TCOD_list_push(onGrants, grant_init_structrecord(grant_new(), each));
        } else if(STREQ(n, "revoke")) {
          TCOD_list_push(onRevokes, revoke_init_structrecord(revoke_new(), each));
        }
      );
      onMsg = structrecord_has_prop(kid, "message") ? structrecord_get_prop_value(kid, "message").s : NULL;
      onDesc = structrecord_has_prop(kid, "description") ? structrecord_get_prop_value(kid, "description").s : NULL;
    } else if(STREQ(structrecord_type(kid), "off")) {
      //add to offStuff
      TS_LIST_FOREACH(subkids,
        char *n = structrecord_type(each);
        if(STREQ(n, "grant")) {
          TCOD_list_push(offGrants, grant_init_structrecord(grant_new(), each));
        } else if(STREQ(n, "revoke")) {
          TCOD_list_push(offRevokes, revoke_init_structrecord(revoke_new(), each));
        }
      );
      offMsg = structrecord_has_prop(kid, "message") ? structrecord_get_prop_value(kid, "message").s : NULL;
    }
  }  
  return status_init(s, structrecord_name(sr), 
    onRevokes, 
    onGrants, 
    onMoves, 
    onDesc, 
    onMsg, 

    offRevokes, 
    offGrants, 
    offMsg
  );
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

EffectFeed effect_feed_init_structrecord(EffectFeed f, StructRecord sr, char *defaultTarget) {
  char *eater = structrecord_name(sr) ? structrecord_name(sr) : defaultTarget;
  char *food = structrecord_has_prop(sr, "food") ? structrecord_get_prop_value(sr, "food").s : "self";
  float volume = structrecord_has_prop(sr, "volume") ? structrecord_get_prop_value(sr, "volume").f : 0.5;
  float digestTime = structrecord_has_prop(sr, "digest_time") ? structrecord_get_prop_value(sr, "digest_time").f : 60;
  return effect_feed_init(f, volume, digestTime, eater, food);
}
EffectPickUp effect_pick_up_init_structrecord(EffectPickUp f, StructRecord sr, char *defaultTarget) {
  char *obj = structrecord_name(sr) ? structrecord_name(sr) : "self";
  char *carrier = structrecord_has_prop(sr, "carrier") ? structrecord_get_prop_value(sr, "carrier").s : defaultTarget;
  return effect_pick_up_init(f, obj, carrier);
}
EffectPutDown effect_put_down_init_structrecord(EffectPutDown f, StructRecord sr, char *defaultTarget) {
  char *obj = structrecord_name(sr) ? structrecord_name(sr) : "self";
  char *carrier = structrecord_has_prop(sr, "carrier") ? structrecord_get_prop_value(sr, "carrier").s : defaultTarget;
  return effect_put_down_init(f, obj, carrier);
}
EffectGrab effect_grab_init_structrecord(EffectGrab f, StructRecord sr, char *defaultTarget) {
  char *obj = structrecord_name(sr) ? structrecord_name(sr) : "self";
  char *grabber = structrecord_has_prop(sr, "grabber") ? structrecord_get_prop_value(sr, "grabber").s : defaultTarget;
  return effect_grab_init(f, obj, grabber);
}
EffectLetGo effect_let_go_init_structrecord(EffectLetGo f, StructRecord sr, char *defaultTarget) {
  char *obj = structrecord_name(sr) ? structrecord_name(sr) : "self";
  char *grabber = structrecord_has_prop(sr, "grabber") ? structrecord_get_prop_value(sr, "grabber").s : defaultTarget;
  return effect_let_go_init(f, obj, grabber);
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

EffectMessage effect_message_init_structrecord(EffectMessage msg, StructRecord sr, char *defaultTarget) {
  char *message = structrecord_get_prop_value(sr, "message").s;
  char *target = structrecord_has_prop(sr, "target") ? structrecord_get_prop_value(sr, "target").s : defaultTarget;
  char *sensor = structrecord_has_prop(sr, "sensor") ? structrecord_get_prop_value(sr, "sensor").s : NULL;
  return effect_message_init(msg, message, target, sensor);
}

Action action_shared_init_structrecord(Action a, StructRecord sr, Flagset triggers, FlagSchema trigSchema, char *defaultTarget) {
  TCOD_list_t conditions = TCOD_list_new();
  TCOD_list_t grants = TCOD_list_new();
  TCOD_list_t revokes = TCOD_list_new();
  TCOD_list_t sets = TCOD_list_new();
  TCOD_list_t msgs = TCOD_list_new();
  for(int i = 0; i < TCOD_list_size(structrecord_children(sr)); i++) {
    StructRecord kid = TCOD_list_get(structrecord_children(sr), i);
    char *kidType = structrecord_type(kid);
    if(STREQ(kidType, "condition")) {
      TCOD_list_push(conditions, condition_init_structrecord(condition_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "grant")) {
      TCOD_list_push(grants, effect_grantrevoke_init_structrecord(effect_grantrevoke_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "revoke")) {
      TCOD_list_push(revokes, effect_grantrevoke_init_structrecord(effect_grantrevoke_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "set")) {
      TCOD_list_push(sets, effect_set_init_structrecord(effect_set_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "message")) {
      TCOD_list_push(msgs, effect_message_init_structrecord(effect_message_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "feed")) {
      TCOD_list_push(msgs, effect_feed_init_structrecord(effect_feed_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "pick_up")) {
      TCOD_list_push(msgs, effect_pick_up_init_structrecord(effect_pick_up_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "put_down")) {
      TCOD_list_push(msgs, effect_put_down_init_structrecord(effect_put_down_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "grab")) {
      TCOD_list_push(msgs, effect_grab_init_structrecord(effect_grab_new(), kid, defaultTarget));
    } else if(STREQ(kidType, "let_go")) {
      TCOD_list_push(msgs, effect_let_go_init_structrecord(effect_let_go_new(), kid, defaultTarget));
    }
  }
  return action_init(a, structrecord_name(sr), triggers, trigSchema, conditions, grants, revokes, sets, msgs);
}

Action action_init_structrecord(Action a, StructRecord sr, FlagSchema trigSchema, char *defaultTarget) {
  Flagset triggers = flagset_init(flagset_new(trigSchema), trigSchema);
  TCOD_list_t trigLabels = flagschema_get_labels(trigSchema);
  TS_LIST_FOREACH(trigLabels,
    if(structrecord_is_flag_set(sr, each)) {
      flagset_set_index(triggers, trigSchema, __i, 1);
    } else {
      flagset_set_index(triggers, trigSchema, __i, 0);
    }
  );
  a = action_shared_init_structrecord(a, sr, triggers, trigSchema, defaultTarget);
  TCOD_list_clear_and_delete(trigLabels);
  return a;
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

TCOD_list_t descactions_init_structrecord(TCOD_list_t actions, StructRecord sr, FlagSchema actionTriggers, char *target) {
  TCOD_list_t triggers = flagschema_get_labels(actionTriggers);
  static TCOD_list_t onLabels = NULL;
  if(!onLabels) { 
    onLabels = TCOD_list_new();
    TS_LIST_FOREACH(triggers,
      char *label=NULL;
      asprintf(&label, "%s_desc", each);
      TCOD_list_push(onLabels, strdup(label));
      free(label);
    );
  }
  TS_LIST_FOREACH(onLabels,
    if(structrecord_has_prop(sr, each)) {
      TCOD_list_t msgs = TCOD_list_new();
      TCOD_list_push(msgs, effect_message_init(effect_message_new(), structrecord_get_prop_value(sr, each).s, target, NULL));
      Flagset flags = flagset_init(flagset_new(actionTriggers), actionTriggers);
      flagset_set_index(flags, actionTriggers, __i, 1);
      TCOD_list_push(actions, action_init(action_new(), each, flags, actionTriggers, NULL, NULL, NULL, NULL, msgs, NULL, NULL, NULL, NULL, NULL));
    }
  );
  TCOD_list_clear_and_delete(triggers);
  return actions;
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
  //on_enter_desc, on_bump_desc, on_walk_up_desc, etc...
  //{trigger}_desc=X needs to get turned into action "{trigger}_desc" { trigger  messages=[X] } 
  descactions_init_structrecord(actions, sr, actionTriggers, "object");

  TileInfo ti = tileinfo_init(tileinfo_new(), l, actions, drawInfos, moveInfos, moveDefaultAllowed, stairs, pit);
  return tile_init(t, wallTransp, floorTransp, ceilTransp, ti);
}

mapVec mapvec_make_int_list(TCOD_list_t l) {
  int sx = (int)TCOD_list_get(l, 0);
  int sy = (int)TCOD_list_get(l, 1);
  int sz = (int)TCOD_list_get(l, 2);
  return (mapVec){sx, sy, sz};
}

mapVec mapvec_make_float_list(TCOD_list_t l) {
  void *px = TCOD_list_get(l, 0);
  float x = *(float *)(&px);
  void *py = TCOD_list_get(l, 1);
  float y = *(float *)(&py);
  void *pz = TCOD_list_get(l, 2);
  float z = *(float *)(&pz);
  return (mapVec){x, y, z};
}

Map map_init_structrecord(Map m, StructRecord sr) {
  //dimensions, ambient light, tilemap
  //implement these search methods
  int ambientLight = structrecord_get_prop_value(sr, "ambient_light").i;
  TCOD_list_t dimensions = structrecord_get_prop_value(sr, "dimensions").list;
  mapVec mapSize = mapvec_make_int_list(dimensions);
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

Volume volume_init_structrecord(Volume v, StructRecord sr) {
  TCOD_list_t pos = structrecord_has_prop(sr, "position") ? structrecord_get_prop_value(sr, "position").list : NULL;
  mapVec position = pos ? mapvec_make_int_list(pos) : (mapVec){0,0,0};
  TCOD_list_t face = structrecord_has_prop(sr, "facing") ? structrecord_get_prop_value(sr, "facing").list : NULL;
  mapVec facing = face ? mapvec_make_float_list(face) : (mapVec){1,0,0};
  TCOD_list_t sz = structrecord_has_prop(sr, "extent") ? structrecord_get_prop_value(sr, "extent").list : NULL;
  mapVec extent = sz ? mapvec_make_float_list(sz) : (mapVec){5,5,5};
  char *type = structrecord_type(sr);
  if(STREQ(type, "sphere")) {
    float radius = structrecord_has_prop(sr, "radius") ? structrecord_get_prop_value(sr, "radius").f : 5;
    return sphere_init(v, position, radius);
  } else if(STREQ(type, "frustum")) {
    int xfov = structrecord_has_prop(sr, "xfov") ? structrecord_get_prop_value(sr, "xfov").i : 2;
    int zfov = structrecord_has_prop(sr, "zfov") ? structrecord_get_prop_value(sr, "zfov").i : 2;
    int near = structrecord_has_prop(sr, "near") ? structrecord_get_prop_value(sr, "near").i : 0;
    int far = structrecord_has_prop(sr, "far") ? structrecord_get_prop_value(sr, "far").i : 10;
    return frustum_init(v, position, facing, xfov, zfov, near, far);
  } else if(STREQ(type, "box")) {
    return box_init(v, position, facing, extent);
  } else if(STREQ(type, "aabox")) {
    return aabox_init(v, position, extent);
  }
  return NULL;
}

Sensor sensor_init_structrecord(Sensor s, StructRecord sr) {
  //Sensor sensor_init(Sensor s, char *id, Volume volume, void *context);
  char *id = structrecord_name(sr) ? structrecord_name(sr) : "sensor";
  #warning dangerous assumption that sr will have 1 and only 1 child?
  StructRecord kid = TCOD_list_get(structrecord_children(sr), 0);
  Volume v = volume_init_structrecord(volume_new(), kid);
  return sensor_init(s, id, v, NULL);
}

Action sugaraction_init_structrecord(Action a, StructRecord sr, char *trigName, Loader l, char *defaultTarget) {
  Flagset triggers = loader_make_trigger(l, trigName);
  return action_shared_init_structrecord(a, sr, triggers, loader_trigger_schema(l), defaultTarget);
}

#define SUGAR_ADD_REQUIRED_EFFECT(_list, _l, _sr, _mode, _evt, _trig, _effect, _rest...) \
do {                                                                                     \
  StructRecord __kid = structrecord_first_child_of_type(_sr, #_evt);                     \
  Action __a;                                                                            \
  if(!__kid) {                                                                           \
    __a = action_init(action_new(), #_mode "_" #_evt,                                    \
      loader_make_trigger(_l, "on_" #_trig), loader_trigger_schema(_l),                  \
      NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL                         \
    );                                                                                   \
  } else {                                                                               \
    __a = sugaraction_init_structrecord(action_new(), __kid,                             \
      "on_" #_trig, _l, "self"                                                           \
    );                                                                                   \
  }                                                                                      \
  /*does kid itself have the desired effect?  If so, use it; */                          \
  /*otherwise, add a pick_up effect to the action.           */                          \
  if(action_effect_##_effect(__a) == NULL) {                                             \
    action_set_effect_##_effect(__a,                                                     \
      effect_##_effect##_init(effect_##_effect##_new(), _rest)                           \             
    );                                                                                   \
  }                                                                                      \
  TCOD_list_push(_list, __a);                                                            \
} while(0)

#define SUGAR_ADD(_list, _l, _sr, _label, _trig) do {                                          \
  StructRecord __kid = structrecord_first_child_of_type(_sr, #_label)                          \
  if(__kid) {                                                                                  \
    Action __a = sugaraction_init_structrecord(action_new(), __kid, "on_" #_trig, _l, "self"); \
    TCOD_list_push(_list, __a);                                                                \
  }                                                                                            \
} while(0)

TCOD_list_t chomp_init_structrecord(TCOD_list_t actions, StructRecord sr, Loader l) {
  char *n = structrecord_name(sr);
  if(STREQ(n, "eat")) {
    float foodVolume = structrecord_has_prop(sr, "volume") ? structrecord_get_prop_value(sr, "volume").f : 0.5;
    int digestionTime = structrecord_has_prop(sr, "digest_time") ? structrecord_get_prop_value(sr, "digest_time").f : 60;
    SUGAR_ADD_REQUIRED_EFFECT(actions, l, sr, carry, start, chomp, feed, 
      foodVolume, digestionTime, "other", "self"
    );
    SUGAR_ADD(actions, l, sr, step, digest);
    SUGAR_ADD(actions, l, sr, end, digested);
  } else if(STREQ(n, "carry")) {
    SUGAR_ADD_REQUIRED_EFFECT(actions, l, sr, carry, start, chomp, pick_up, 
      "self", "other"
    );
    SUGAR_ADD_REQUIRED_EFFECT(actions, l, sr, carry, release, unchomp, put_down, 
      "self", "other"
    );
    SUGAR_ADD(actions, l, sr, step, carry);
    SUGAR_ADD(actions, l, sr, end, release);
    SUGAR_ADD(actions, l, sr, left, carry_left);
    SUGAR_ADD(actions, l, sr, right, carry_right);
    SUGAR_ADD(actions, l, sr, forward, carry_forward);
    SUGAR_ADD(actions, l, sr, back, carry_back);
    SUGAR_ADD(actions, l, sr, up, carry_up);
    SUGAR_ADD(actions, l, sr, down, carry_down);
  } else if(STREQ(n, "latch")) {
    SUGAR_ADD_REQUIRED_EFFECT(actions, l, sr, latch, start, chomp, grab, 
      "self", "other"
    );
    SUGAR_ADD_REQUIRED_EFFECT(actions, l, sr, latch, release, unchomp, let_go, 
      "self", "other"
    );
    SUGAR_ADD(actions, l, sr, step, latch);
    SUGAR_ADD(actions, l, sr, end, release);
    SUGAR_ADD(actions, l, sr, left, tug_left);
    SUGAR_ADD(actions, l, sr, right, tug_right);
    SUGAR_ADD(actions, l, sr, forward, tug_forward);
    SUGAR_ADD(actions, l, sr, back, tug_back);
    SUGAR_ADD(actions, l, sr, up, tug_up);
    SUGAR_ADD(actions, l, sr, down, tug_down);
  }
  return actions;
}
#undef SUGAR_ADD_REQUIRED_EFFECT
#undef SUGAR_ADD

Object object_init_structrecord_overrides(Object o, Loader l, StructRecord base, StructRecord over) {
  #warning unsafe assumption that override will only add and not change data
  //to solve later, implement structrecord_combine() and do: 
  //s = structrecord_combine(structrecord_combine(structrecord_new(), sr), over)
  //and work from there. structrecord_combine shall be recursive and appropriate.
  //also change the code below to refer only to the combined, new sr
  StructRecord sr = base;
  MoveInfo mi = NULL;
  TCOD_list_t drawInfos = TCOD_list_new();
  TCOD_list_t sensors = TCOD_list_new();
  TCOD_list_t actions = TCOD_list_new();
  TCOD_list_t kids = structrecord_children(sr);
  int drawIndex = 0;
  TS_LIST_FOREACH(kids,
    char *t = structrecord_type(each);
    if(STREQ(t, "movement")) {
      mi=moveinfo_init_structrecord(moveinfo_new(), each);
    } else if(STREQ(t, "draw")) {
      TCOD_list_push(drawInfos, drawinfo_init_structrecord(drawinfo_new(), each, drawIndex, &drawIndex));
      drawIndex++;
    } else if(STREQ(t, "sensor")) {
      TCOD_list_push(sensors, sensor_init_structrecord(sensor_new(), each));
    } else if(STREQ(t, "action")) {
      TCOD_list_push(actions, action_init_structrecord(action_new(), each, loader_trigger_schema(l), "self"));
    } else if(STREQ(t, "chomp")) {
      chomp_init_structrecord(actions, each, l);
    }
  );
  #warning should carry strength also be a loaded variable?

  int weight = structrecord_has_prop(sr, "weight") ? structrecord_get_prop_value(sr, "weight").i : 125;
  
  TCOD_list_t pos = structrecord_has_prop(over, "position") ? structrecord_get_prop_value(over, "position").list : NULL;
  mapVec position = pos ? mapvec_make_int_list(pos) : (mapVec) {0, 0, 0};
  TCOD_list_t face = structrecord_has_prop(over, "facing") ? structrecord_get_prop_value(over, "facing").list : NULL;
  mapVec facing = face ? mapvec_make_float_list(face) : (mapVec) {1, 0, 0};
  char *mapName = structrecord_get_prop_value(over, "map").s;
  Map map = loader_get_map(l, mapName);
  char *id = structrecord_get_prop_value(over, "id").s;
  
  char *desc = structrecord_has_prop(over, "description") ? structrecord_get_prop_value(over, "description").s : NULL;
  
  descactions_init_structrecord(actions, sr, loader_trigger_schema(l), "other");
  
  ObjectInfo oi = objectinfo_init(objectinfo_new(), l, drawInfos, mi, actions, ctype, foodVolume, digestionTime, weight, desc);
  o = object_init(o, id, position, facing, map, oi);
  TS_LIST_FOREACH(sensors, object_add_sensor(o, each));
  map_add_object(map, o);
  return o;
}