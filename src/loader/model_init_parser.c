#include "loader/model_init_parser.h"

TCOD_parser_struct_t moveinfo_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_list_t moveFlags = loader_move_flags(l);
  TCOD_parser_struct_t movest = TCOD_parser_new_struct(p, "movement");
  TS_LIST_FOREACH(moveFlags,
    TCOD_struct_add_property(movest, each, TCOD_TYPE_BOOL, false);
  );
  return movest;
}

TCOD_parser_struct_t drawinfo_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t drawst = TCOD_parser_new_struct(p, "draw");
  TCOD_struct_add_property(drawst, "z", TCOD_TYPE_INT, false); //defaults to the index of the drawst.
  TCOD_struct_add_property(drawst, "fore", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "back", TCOD_TYPE_COLOR, false);
  TCOD_struct_add_property(drawst, "symbol", TCOD_TYPE_CHAR, true);
  return drawst;
}

TCOD_parser_struct_t sphere_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t spherest = TCOD_parser_new_struct(p, "sphere");
  TCOD_struct_add_list_property(spherest, "position", TCOD_TYPE_INT, false); //defaults to [0,0,0]
  TCOD_struct_add_property(spherest, "radius", TCOD_TYPE_FLOAT, false); //defaults to 5
  return spherest;
}

TCOD_parser_struct_t frustum_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t frustumst = TCOD_parser_new_struct(p, "frustum");
  TCOD_struct_add_list_property(frustumst, "position", TCOD_TYPE_INT, false); //defaults to [0,0,0]
  TCOD_struct_add_list_property(frustumst, "facing", TCOD_TYPE_FLOAT, false); //defaults to [1,0,0]
  TCOD_struct_add_property(frustumst, "xfov", TCOD_TYPE_INT, false); //defaults to 2
  TCOD_struct_add_property(frustumst, "zfov", TCOD_TYPE_INT, false); //defaults to 2
  TCOD_struct_add_property(frustumst, "near", TCOD_TYPE_INT, false); //defaults to 0
  TCOD_struct_add_property(frustumst, "far", TCOD_TYPE_INT, false); //defaults to 5
  return frustumst;
}

TCOD_parser_struct_t box_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t boxst = TCOD_parser_new_struct(p, "box");
  TCOD_struct_add_list_property(boxst, "position", TCOD_TYPE_INT, false); //defaults to [0,0,0]
  TCOD_struct_add_list_property(boxst, "facing", TCOD_TYPE_FLOAT, false); //defaults to [1,0,0]
  TCOD_struct_add_list_property(boxst, "extent", TCOD_TYPE_FLOAT, false); //defaults to [5,5,5]
  return boxst;
}

TCOD_parser_struct_t aabox_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t aaboxst = TCOD_parser_new_struct(p, "aabox");
  TCOD_struct_add_list_property(aaboxst, "position", TCOD_TYPE_INT, false); //defaults to [0,0,0]
  TCOD_struct_add_list_property(aaboxst, "extent", TCOD_TYPE_FLOAT, false); //defaults to [5,5,5]
  return aaboxst;
}

TCOD_parser_struct_t sensor_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t spherest = sphere_init_parser(p, l);
  TCOD_parser_struct_t frustumst = frustum_init_parser(p, l);
  TCOD_parser_struct_t boxst = box_init_parser(p, l);
  TCOD_parser_struct_t aaboxst = aabox_init_parser(p, l);
  
  TCOD_parser_struct_t sensorst = TCOD_parser_new_struct(p, "sensor");
  TCOD_struct_add_structure(sensorst, spherest);
  TCOD_struct_add_structure(sensorst, frustumst);
  TCOD_struct_add_structure(sensorst, boxst);
  TCOD_struct_add_structure(sensorst, aaboxst);
  return sensorst;
}

TCOD_parser_struct_t grant_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t grantst = TCOD_parser_new_struct(p, "grant");
  TCOD_struct_add_property(grantst, "duration", TCOD_TYPE_FLOAT, false); //defaults to infinity
  TCOD_struct_add_property(grantst, "priority", TCOD_TYPE_INT, false); //defaults to 1
  TCOD_struct_add_property(grantst, "reason", TCOD_TYPE_STRING, false); //defaults to NULL
  //this is actually only valid in grants {} on actions
  TCOD_struct_add_property(grantst, "target", TCOD_TYPE_STRING, false);
  return grantst;
}

TCOD_parser_struct_t revoke_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t revokest = TCOD_parser_new_struct(p, "revoke");
  TCOD_struct_add_property(revokest, "priority", TCOD_TYPE_INT, false); //defaults to 1
  TCOD_struct_add_property(revokest, "reason", TCOD_TYPE_STRING, false); //defaults to NULL
  //this is actually only valid in revokes {} on actions
  TCOD_struct_add_property(revokest, "target", TCOD_TYPE_STRING, false);
  return revokest;
}

TCOD_parser_struct_t status_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t grantst = grant_init_parser(p, l);

  TCOD_parser_struct_t revokest = revoke_init_parser(p, l);
  
  TCOD_parser_struct_t movest = moveinfo_init_parser(p, l);
  
  TCOD_parser_struct_t onst = TCOD_parser_new_struct(p, "on");
  TCOD_struct_add_structure(onst, grantst);
  TCOD_struct_add_structure(onst, revokest);
  TCOD_struct_add_structure(onst, movest);
  TCOD_struct_add_property(onst, "description", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(onst, "message", TCOD_TYPE_STRING, false);

  TCOD_parser_struct_t offst = TCOD_parser_new_struct(p, "off");
  TCOD_struct_add_structure(offst, grantst);
  TCOD_struct_add_structure(offst, revokest);
  TCOD_struct_add_property(offst, "message", TCOD_TYPE_STRING, false);
  
  TCOD_parser_struct_t statst = TCOD_parser_new_struct(p, "status");

  TCOD_struct_add_structure(statst, onst);
  TCOD_struct_add_structure(statst, offst);
  return statst;
}

TCOD_parser_struct_t check_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t checkst = TCOD_parser_new_struct(p, "check");
  TCOD_struct_add_property(checkst, "target", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "source", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "source_object", TCOD_TYPE_STRING, false);

  TCOD_struct_add_property(checkst, "greater_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "greater_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "less_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "less_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_flag(checkst, "greater_than");
  TCOD_struct_add_flag(checkst, "greater_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "less_than");
  TCOD_struct_add_flag(checkst, "less_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "equal_to");

  TCOD_struct_add_property(checkst, "starts_with", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "ends_with", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "find_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(checkst, "is_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(checkst, "starts_with");
  TCOD_struct_add_flag(checkst, "ends_with");
  TCOD_struct_add_flag(checkst, "find_string");
  TCOD_struct_add_flag(checkst, "is_string");

  TCOD_struct_add_property(checkst, "count_greater_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_greater_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_less_than", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_less_than_or_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(checkst, "count_equal_to", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_flag(checkst, "count_greater_than");
  TCOD_struct_add_flag(checkst, "count_greater_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "count_less_than");
  TCOD_struct_add_flag(checkst, "count_less_than_or_equal_to");
  TCOD_struct_add_flag(checkst, "count_equal_to");
  
  TCOD_struct_add_property(checkst, "contains", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(checkst, "contains_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(checkst, "contains_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(checkst, "contains_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(checkst, "contains");
  TCOD_struct_add_flag(checkst, "contains_string");
  TCOD_struct_add_flag(checkst, "contains_all");
  TCOD_struct_add_flag(checkst, "contains_all_strings");
  return checkst;
}

TCOD_parser_struct_t condition_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t checkst = check_init_parser(p, l);

  static const char *condition_modes[] = { "all", "any", NULL };    
  TCOD_parser_struct_t conditionst = TCOD_parser_new_struct(p, "condition");
  TCOD_struct_add_value_list(conditionst, "require", condition_modes, false); //defaults to 'all'
  TCOD_struct_add_flag(conditionst, "negate");
  TCOD_struct_add_structure(conditionst, checkst);
  TCOD_struct_add_structure(conditionst, conditionst);  
  return conditionst;
}

TCOD_parser_struct_t effect_set_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t setst = TCOD_parser_new_struct(p, "set");
  TCOD_struct_add_property(setst, "increase", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(setst, "decrease", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(setst, "set_number", TCOD_TYPE_FLOAT, false);
  //srcvar-using variants
  TCOD_struct_add_flag(setst, "increase");
  TCOD_struct_add_flag(setst, "decrease");
  TCOD_struct_add_flag(setst, "set_number");
  
  TCOD_struct_add_property(setst, "concat", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "excise", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "set_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_flag(setst, "concat");
  TCOD_struct_add_flag(setst, "excise");
  TCOD_struct_add_flag(setst, "set_string");

  TCOD_struct_add_property(setst, "push", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "append_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_property(setst, "remove", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "remove_all", TCOD_TYPE_INT, false);
  TCOD_struct_add_list_property(setst, "set_list", TCOD_TYPE_INT, false);  
  TCOD_struct_add_flag(setst, "push");
  TCOD_struct_add_flag(setst, "append_all");
  TCOD_struct_add_flag(setst, "remove");
  TCOD_struct_add_flag(setst, "remove_all");
  TCOD_struct_add_flag(setst, "set_list");

  TCOD_struct_add_property(setst, "push_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "append_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(setst, "remove_string", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "remove_all_strings", TCOD_TYPE_STRING, false);
  TCOD_struct_add_list_property(setst, "set_string_list", TCOD_TYPE_STRING, false);  
  TCOD_struct_add_flag(setst, "push_string");
  TCOD_struct_add_flag(setst, "append_all_strings");
  TCOD_struct_add_flag(setst, "remove_string");
  TCOD_struct_add_flag(setst, "remove_all_strings");
  TCOD_struct_add_flag(setst, "set_string_list");
  return setst;
}

TCOD_parser_struct_t effect_message_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t msgst = TCOD_parser_new_struct(p, "message");
  TCOD_struct_add_property(msgst, "message", TCOD_TYPE_STRING, true);
  TCOD_struct_add_property(msgst, "target", TCOD_TYPE_STRING, false);
  TCOD_struct_add_property(msgst, "sensor", TCOD_TYPE_STRING, false);
  return msgst;
}

TCOD_parser_struct_t sugaraction_init_parser(TCOD_parser_t p, Loader l, char *label) {
  TCOD_parser_struct_t grantst = grant_init_parser(p, l);
  TCOD_parser_struct_t revokest = revoke_init_parser(p, l);
  TCOD_parser_struct_t conditionst = condition_init_parser(p, l);
  TCOD_parser_struct_t setst = effect_set_init_parser(p, l);
  TCOD_parser_struct_t msgst = effect_message_init_parser(p, l);
  
  FlagSchema triggerSchema = loader_trigger_schema(l);
  TCOD_list_t triggers = flagschema_get_labels(triggerSchema);
  TCOD_parser_struct_t actionst = TCOD_parser_new_struct(p, label);
  TS_LIST_FOREACH(triggers,
    TCOD_struct_add_flag(actionst, each);
  );
  TCOD_list_clear_and_delete(triggers);
  //timers not supported yet
  
  TCOD_struct_add_structure(actionst, conditionst);
  TCOD_struct_add_structure(actionst, grantst);  
  TCOD_struct_add_structure(actionst, revokest);  
  TCOD_struct_add_structure(actionst, setst);
  TCOD_struct_add_structure(actionst, msgst);
  return actionst;
}

TCOD_parser_struct_t action_init_parser(TCOD_parser_t p, Loader l) {
  return sugaraction_init_parser(p, l, "action");
}

TCOD_parser_struct_t chomp_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t startst = sugaraction_init_parser(p, l, "start");
  TCOD_parser_struct_t stepst = sugaraction_init_parser(p, l, "step");
  TCOD_parser_struct_t endst = sugaraction_init_parser(p, l, "end");
  TCOD_parser_struct_t leftst = sugaraction_init_parser(p, l, "left");
  TCOD_parser_struct_t rightst = sugaraction_init_parser(p, l, "right");
  TCOD_parser_struct_t backst = sugaraction_init_parser(p, l, "back");
  TCOD_parser_struct_t forwardst = sugaraction_init_parser(p, l, "forward");
  TCOD_parser_struct_t upst = sugaraction_init_parser(p, l, "up");
  TCOD_parser_struct_t downst = sugaraction_init_parser(p, l, "down");
  
  TCOD_parser_struct_t chompst = TCOD_parser_new_struct(p, "chomp");
  TCOD_struct_add_property(chompst, "volume", TCOD_TYPE_FLOAT, false);
  TCOD_struct_add_property(chompst, "digest_time", TCOD_TYPE_FLOAT, false);

  TCOD_struct_add_structure(chompst, startst);
  TCOD_struct_add_structure(chompst, stepst);
  TCOD_struct_add_structure(chompst, endst);
  TCOD_struct_add_structure(chompst, leftst);
  TCOD_struct_add_structure(chompst, rightst);
  TCOD_struct_add_structure(chompst, backst);
  TCOD_struct_add_structure(chompst, forwardst);
  TCOD_struct_add_structure(chompst, upst);
  TCOD_struct_add_structure(chompst, downst);
  
  return chompst;
  //maybe a sugaraction_init_parser() of some sort; on sugaraction_init_structrecord, take an implicit triggerset & effect list?
  /*
  the necessary sugar actions are:
  
  start
  step
  end
  
  left
  right
  back
  forward
  up
  down

  and their interpretation will be a matter for object_init_structrecord to handle based on the name of the chomp struct.
  
  the chomp struct defines these sugaractions along with volume= and digest_time= for food.

  chomp "eat" { 
    //implicitly on_chomp, on_digest, on_digested
    volume=0.5 //portion of full stomach
    digest_time=30 //seconds
  	start { ... } //implicit 'feed { food="self" eater="other" volume=... digest_time=..}' effect based on volume and digest_time above
  	step { ... }
  	end { ... }
  }
  chomp "carry" { 
    //implicitly on_chomp, on_carry, on_move_*, on_release
    start { ... } //implicit 'pick_up' effect
    step { ... }
  	left { ... }
  	right { ... } 
  	back { ... } 
  	forward { ... }
  	up { ... } 
  	down { ... }
    end { ... }
  }
  chomp "latch" { 
    //implicitly on_chomp, on_latch, on_tug_*, on_release
  	start { ... } //implicit 'latch' effect
  	step { ... } 
  	left { ... }
  	right { ... } 
  	back { ... } 
  	forward { ... }
  	//tugging up and tugging down are impossible
  	end { ... }
  }
  */
}

TCOD_parser_struct_t object_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t movest = moveinfo_init_parser(p, l);
  TCOD_parser_struct_t drawst = drawinfo_init_parser(p, l);
  TCOD_parser_struct_t sensorst = sensor_init_parser(p, l);
  
  TCOD_parser_struct_t actionst = action_init_parser(p, l);

  TCOD_parser_struct_t chompst = chomp_init_parser(p, l);
  
  TCOD_parser_struct_t objectst = TCOD_parser_new_struct(p, "object");
  TCOD_struct_add_structure(objectst, sensorst);
  TCOD_struct_add_structure(objectst, movest);
  TCOD_struct_add_structure(objectst, drawst);
  TCOD_struct_add_structure(objectst, actionst);
  TCOD_struct_add_structure(objectst, chompst);
  
  TCOD_struct_add_property(objectst, "weight", TCOD_TYPE_INT, false); //defaults to 125 grams, apprx the average weight of a salamander

  //desc
  triggerdesc_init_parser(p, objectst, l);
  
  TCOD_struct_add_property(objectst, "description", TCOD_TYPE_STRING, false); //defaults to "".
  return objectst;
}

void triggerdesc_init_parser(TCOD_parser_t p, TCOD_parser_struct_t st, Loader l) {
  FlagSchema triggerSchema = loader_trigger_schema(l);
  TCOD_list_t triggers = flagschema_get_labels(triggerSchema);
  TS_LIST_FOREACH(triggers,
    char *label=NULL;
    asprintf(&label, "%s_desc", each);
    TCOD_struct_add_property(st, strdup(label), TCOD_TYPE_STRING, false);
    free(label);
  );
  TCOD_list_clear_and_delete(triggers);
}

TCOD_parser_struct_t tile_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t actionst = action_init_parser(p, l);
  TCOD_parser_struct_t movst = moveinfo_init_parser(p, l);
  TCOD_parser_struct_t drawst = drawinfo_init_parser(p, l);

  static const char *movement_defaults[] = { "allow", "deny", NULL };    

  TCOD_parser_struct_t tst = TCOD_parser_new_struct(p, "tile");
  //opacity
  TCOD_struct_add_list_property(tst, "opacity", TCOD_TYPE_CHAR, false); //defaults to [0,0,0,0,15,15,0,0] -- an opaque floor, no ceiling
  //opacity shorthands
  TCOD_struct_add_property(tst, "wall_opacity", TCOD_TYPE_CHAR, false); //defaults to 0
  TCOD_struct_add_property(tst, "floor_opacity", TCOD_TYPE_CHAR, false); //defaults to 15
  TCOD_struct_add_property(tst, "ceiling_opacity", TCOD_TYPE_CHAR, false); //defaults to 15
  TCOD_struct_add_property(tst, "uniform_opacity", TCOD_TYPE_CHAR, false); //defaults to 0

  //desc
  triggerdesc_init_parser(p, tst, l);

  //stairs
  TCOD_struct_add_flag(tst, "stairs");

  //movement
  TCOD_struct_add_value_list(tst, "movement_default", movement_defaults, false); //defaults to "allow"
  TCOD_struct_add_structure(tst, movst);
  
  //actions
  TCOD_struct_add_structure(tst, actionst);  
  
  //drawing
  TCOD_struct_add_structure(tst, drawst);
  return tst;
}

TCOD_parser_struct_t map_init_parser(TCOD_parser_t p, Loader l) {
  TCOD_parser_struct_t mapst = TCOD_parser_new_struct(p, "map");
  TCOD_struct_add_property(mapst, "ambient_light", TCOD_TYPE_CHAR, false); //defaults to 8
  TCOD_struct_add_list_property(mapst, "dimensions", TCOD_TYPE_INT, true); //no default
  TCOD_struct_add_list_property(mapst, "tilemap", TCOD_TYPE_CHAR, true); //no default
  return mapst;
}