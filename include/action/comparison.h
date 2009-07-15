#ifndef _COMPARISON_H
#define _COMPARISON_H

#include <libtcod.h>

typedef enum {
  ComparisonNone,
  GreaterThan,
  LessThan,
  EqualTo,
  GreaterThanOrEqualTo,
  LessThanOrEqualTo,
  
  CountGreaterThan,
  CountLessThan,
  CountEqualTo,
  CountGreaterThanOrEqualTo,
  CountLessThanOrEqualTo,
  
  ContainsNumber,
  
  StartsWith,
  EndsWith,
  FindString,
  IsString,
  ContainsString,
  
  ContainsAll,
  ContainsAllString
} ComparisonMode;

struct _comparison {
  ComparisonMode mode;
  TCOD_value_t *value; //may be undefined, in which case pass the value
};
typedef struct _comparison *Comparison;

ComparisonMode comparison_mode_from_name(char *n);

Comparison comparison_new();
Comparison comparison_init(Comparison c, ComparisonMode mode, TCOD_value_t *val);
void comparison_free(Comparison c);

bool comparison_expects_rval(Comparison c);

bool comparison_result(Comparison c, TCOD_value_t *lval, TCOD_value_t *rval);


#endif