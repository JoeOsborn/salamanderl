#ifndef _STRUCTRECORD_H
#define _STRUCTRECORD_H

#include <libtcod.h>
#include "loader/prop.h"

//this is similar to TCOD_struct_int_t, but I know more about how it's created and
//destroyed, and I don't need to worry about it getting mangled when a parser is destroyed.

struct _structrecord {
  char *type;
  char *name;
  TCOD_list_t flags; //list of char *
  TCOD_list_t props; //list of props
  TCOD_list_t children; //list of struct_record*
  
  struct _structrecord *parent; //parent, if any
};
typedef struct _structrecord *StructRecord;

StructRecord structrecord_new();
StructRecord structrecord_init(StructRecord sr, char *typeName, char *name, StructRecord parent);
void structrecord_free(StructRecord sr);
char *structrecord_type(StructRecord sr);
char *structrecord_name(StructRecord sr);
void structrecord_set_name(StructRecord sr, char *n);
TCOD_list_t structrecord_flags(StructRecord sr);
void structrecord_add_flag(StructRecord sr, char *flag);
bool structrecord_has_prop(StructRecord sr, char *propName);
TCOD_list_t structrecord_props(StructRecord sr);
void structrecord_add_prop(StructRecord sr, Prop p);
TCOD_list_t structrecord_children(StructRecord sr);
void structrecord_add_child(StructRecord sr, StructRecord kid);
StructRecord structrecord_first_child_of_type(StructRecord sr, char *type);
StructRecord structrecord_parent(StructRecord sr);
void structrecord_set_parent(StructRecord sr, StructRecord parent);

bool structrecord_is_flag_set(StructRecord sr, char *flag);
Prop structrecord_get_prop_named(StructRecord sr, char *name);
TCOD_value_type_t structrecord_get_prop_type(StructRecord sr, char *name);
TCOD_value_t structrecord_get_prop_value(StructRecord sr, char *name);
TCOD_value_t structrecord_get_prop_value_default(StructRecord sr, char *name, TCOD_value_t val);
#endif