#include "loader/structrecord.h"
#include <string.h>

StructRecord structrecord_new() {
  return calloc(1, sizeof(struct _structrecord));
}
StructRecord structrecord_init(StructRecord sr, char *typeName, char *name, StructRecord parent) {
  sr->type=strdup(typeName);
  sr->name = name ? strdup(name) : NULL;
  if(parent) {
    structrecord_add_child(parent, sr);
  }
  sr->flags=TCOD_list_new();
  sr->props=TCOD_list_new();
  sr->children=TCOD_list_new();
  return sr;
}
void structrecord_free(StructRecord sr) {
  free(sr->type);
  if(sr->name) {
    free(sr->name);
  }
  for(int i = 0; i < TCOD_list_size(sr->flags); i++) {
    free(TCOD_list_get(sr->flags, i));
  }
  TCOD_list_delete(sr->flags);
  for(int i = 0; i < TCOD_list_size(sr->props); i++) {
    prop_free(TCOD_list_get(sr->props, i));
  }
  TCOD_list_delete(sr->props);
  for(int i = 0; i < TCOD_list_size(sr->children); i++) {
    structrecord_free(TCOD_list_get(sr->children, i));
  }
  TCOD_list_delete(sr->children);
  sr->parent = NULL;
  free(sr);
}
char *structrecord_type(StructRecord sr) {
  return sr->type;
}
char *structrecord_name(StructRecord sr) {
  return sr->name;
}
TCOD_list_t structrecord_flags(StructRecord sr) {
  return sr->flags;
}
void structrecord_add_flag(StructRecord sr, char *flag) {
  TCOD_list_push(sr->flags, strdup(flag));
}
TCOD_list_t structrecord_props(StructRecord sr) {
  return sr->props;
}
bool structrecord_is_flag_set(StructRecord sr, char *name) {
  for(int i = 0; i < TCOD_list_size(sr->flags); i++) {
    char *flag = TCOD_list_get(sr->flags, i);
    if(strcmp(flag, name) == 0) {
      return true;
    }
  }
  return false;
}
Prop structrecord_get_prop_named(StructRecord sr, char *name) {
  for(int i = 0; i < TCOD_list_size(sr->props); i++) {
    Prop p = TCOD_list_get(sr->props, i);
    if(strcmp(prop_name(p), name) == 0) {
      return p;
    }
  }
  return NULL;
}
TCOD_value_type_t structrecord_get_prop_type(StructRecord sr, char *name) {
  Prop p = structrecord_get_prop_named(sr, name);
  return p ? prop_type(p) : TCOD_TYPE_NONE;
}
TCOD_value_t structrecord_get_prop_value(StructRecord sr, char *name) {
  Prop p = structrecord_get_prop_named(sr, name);
  if(!p) { abort(); }
  return prop_value(p);
}
TCOD_value_t structrecord_get_prop_value_default(StructRecord sr, char *name, TCOD_value_t val) {
  Prop p = structrecord_get_prop_named(sr, name);
  TCOD_value_t result = val;
  if(p) { result = prop_value(p); }
  return result;
}
void structrecord_add_prop(StructRecord sr, Prop p) {
  TCOD_list_push(sr->props, p);
}
TCOD_list_t structrecord_children(StructRecord sr) {
  return sr->children;
}
void structrecord_add_child(StructRecord sr, StructRecord kid) {
  TCOD_list_push(sr->children, kid);
  structrecord_set_parent(kid, sr);
}
StructRecord structrecord_parent(StructRecord sr) {
  return sr->parent;
}
void structrecord_set_parent(StructRecord sr, StructRecord parent) {
  sr->parent = parent;
}