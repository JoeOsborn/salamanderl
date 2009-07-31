#include "action/bindings.h"
#include <string.h>
#include <stdlib.h>
#include <tilesense.h>

Bindings bindings_new() {
  return calloc(1, sizeof(struct _bindings));
}
Bindings bindings_init(Bindings b, Bindings parent, char *label, void *value, TCOD_list_t subbindings) {
  b->parent = parent;
  b->label = label ? strdup(label) : NULL;
  b->value = value;
  b->subbindings = subbindings ? subbindings : TCOD_list_new();
  TS_LIST_FOREACH(b->subbindings, ((Bindings)each)->parent = b; );
  return b;
}
void bindings_free(Bindings b) {
  free(b->label);
  TS_LIST_CLEAR_AND_DELETE(b->subbindings, bindings);
  free(b);
}

void bindings_fill_from(Bindings b1, Bindings b2) {
  Bindings sub = NULL;
  TS_LIST_FOREACH(b2->subbindings, 
    sub = bindings_insert(b1, ((Bindings)each)->label, NULL);
    bindings_set_value(sub, ((Bindings)each)->value);
    bindings_fill_from(sub, each);
  );
}

Bindings bindings_find_path(Bindings b, char *key) {
  char *next = strchr(key, '.');
  if(next) { next++; }
  for(int i = 0; i < TCOD_list_size(b->subbindings); i++) {
    Bindings sub = TCOD_list_get(b->subbindings, i);
    char *check = sub->label;
    if(strncmp(key, check, next-key-1) == 0) {
      if(next) {
        return bindings_find_path(sub, next);
      } else {
        return sub;
      }
    }
  }
  return NULL;
}
Bindings bindings_insert(Bindings b, char *path, void *value) {
  Bindings preexisting = bindings_find_path(b, path);
  if(preexisting != NULL) { return preexisting; }
  //later, insert in the alphabetical middle, then update find_path to do a binary search.
  char *next = strchr(path, '.');
  if(next) { next++; }
  if(!next) {
    Bindings bind = bindings_init(bindings_new(), b, path, value, NULL);
    TCOD_list_push(b->subbindings, bind);
    return bind;
  } else { 
    //there's another level from path--next; find the right one or make it if necessary
    char *firstPart = calloc(next-path, sizeof(char));
    strncpy(firstPart, path, next-path-1);
    Bindings treeTop = bindings_find_path(b, firstPart);
    if(!treeTop) {
      treeTop = bindings_init(bindings_new(), b, firstPart, NULL, NULL);
      //later, insert alphabetically
      TCOD_list_push(b->subbindings, treeTop);
    }
    free(firstPart);
    return bindings_insert(treeTop, next, value);
  }
}

void *bindings_get_value_path(Bindings b, char *path) {
  Bindings entry = bindings_find_path(b, path);
  if(!entry) { return NULL; }
  return entry->value;
}
void bindings_set_value_path(Bindings b, char *path, void *value) {
  Bindings entry = bindings_insert(b, path, value);
  if(!entry) { return; }
  entry->value = value;
}

char *bindings_label(Bindings b) {
  return b->label;
}
void *bindings_value(Bindings b) {
  return b->value;
}
void bindings_set_value(Bindings b, void *val) {
  b->value = val;
}

void bindings_unbind(Bindings b) {
  b->value = NULL;
  TS_LIST_FOREACH(b->subbindings, bindings_unbind(each));
}
Bindings bindings_next_unbound(Bindings b) {
  if(b->value == NULL) { return b; }
  Bindings unbound = NULL;
  TS_LIST_FOREACH(b->subbindings,
    unbound = bindings_next_unbound(each);
    if(unbound) { return unbound; }
  );
  return NULL;
}

Bindings bindings_parent(Bindings b) {
  return b->parent;
}
char *bindings_full_path(Bindings b) {
  if(!(b->parent)) { return strdup(b->label); }
  char *val;
  char *rest = bindings_full_path(b->parent);
  if(b->label) {
    asprintf(&val, "%s.%s", rest, b->label);
  } else {
    asprintf(&val, "%s.%i", rest, bindings_index_of_subbinding(b->parent, b));
  }
  free(rest);
  return val;
}

int bindings_index_of_subbinding(Bindings b, Bindings sub) {
  TS_LIST_FOREACH(b->subbindings, if(each == sub) { return __i; });
  return -1;
}
