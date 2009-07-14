#ifndef _BINDINGS_H
#define _BINDINGS_H

#include <libtcod.h>

struct _bindings {
  char *label;
  void *value;
  TCOD_list_t subbindings;
  struct _bindings *parent;
};

typedef struct _bindings *Bindings;

Bindings bindings_new();
Bindings bindings_init(Bindings b, Bindings parent, char *label, void *value, TCOD_list_t subbindings);
void bindings_free(Bindings b);

void bindings_fill_from(Bindings b1, Bindings b2);

Bindings bindings_find_path(Bindings b, char *key);
Bindings bindings_insert(Bindings b, char *path, void *value);

void *bindings_get_value_path(Bindings b, char *path);
void bindings_set_value_path(Bindings b, char *path, void *value);

char *bindings_label(Bindings b);
void *bindings_value(Bindings b);
void bindings_set_value(Bindings b, void *val);

void bindings_unbind(Bindings b);
Bindings bindings_next_unbound(Bindings b);

Bindings bindings_parent(Bindings b);
void bindings_set_parent(Bindings b, Bindings p);
char *bindings_full_path(Bindings b);

#endif