#include "loader/prop.h"
#include <tilesense.h>

Prop prop_new() {
  return calloc(1, sizeof(struct _prop));
}
Prop prop_init(Prop p, char *name, TCOD_value_type_t type, TCOD_value_t value) {
  p->name=name ? strdup(name) : NULL;
  p->type=type;
  //make a copy
  if(type == TCOD_TYPE_STRING) {
    p->value.s = value.s ? strdup(value.s) : "";
  } else if((type & TCOD_TYPE_LIST) == TCOD_TYPE_LIST) {
    #warning will this be safe for lists of strings?  otoh, will it leak?
    p->value.list = TCOD_list_duplicate(value.list);
  } else {
    p->value = value;
  }
  return p;
}
void prop_free(Prop p) {
  if(p->name) { free(p->name); }
  if(p->type == TCOD_TYPE_STRING) {
    free(p->value.s);
  } else if((p->type & TCOD_TYPE_LIST) == TCOD_TYPE_LIST) {
    //should probably delete list elements if they're strings, etc.
    //should we even bother?  we can know if type & string or type & whatever.
    TCOD_list_delete(p->value.list);
  }
  free(p);
}

char *prop_name(Prop p) {
  return p->name;
}
TCOD_value_type_t prop_type(Prop p) {
  return p->type;
}
TCOD_value_t prop_value(Prop p) {
  return p->value;
}
