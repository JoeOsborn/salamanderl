#ifndef _PROP_H
#define _PROP_H

#include <libtcod.h>

struct _prop {
  char *name;
  TCOD_value_type_t type;
  TCOD_value_t value;
};

typedef struct _prop * Prop;

Prop prop_new();
Prop prop_init(Prop p, char *name, TCOD_value_type_t type, TCOD_value_t value);
void prop_free(Prop p);

char *prop_name(Prop p);
TCOD_value_type_t prop_type(Prop p);
TCOD_value_t prop_value(Prop p);

#endif