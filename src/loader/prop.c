#include "loader/prop.h"

Prop prop_new() {
  return calloc(1, sizeof(struct _prop));
}
Prop prop_init(Prop p, char *name, TCOD_value_type_t type, TCOD_value_t value) {
  p->name=strdup(name);
  p->type=type;
  //make a copy
  if(type == TCOD_TYPE_STRING) {
    p->value.s = strdup(value.s);
  } else if(type == TCOD_TYPE_LIST) {
    p->value.list = TCOD_list_duplicate(value.list);
  } else {
    p->value = value;
  }
  return p;
}
void prop_free(Prop p) {
  free(p->name);
  if(type == TCOD_TYPE_STRING) {
    free(p->value.s);
  } else if(type == TCOD_TYPE_LIST) {
    TCOD_list_clear_and_delete(p->value.list);
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
