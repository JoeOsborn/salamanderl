#ifndef _COMPARISON_H
#define _COMPARISON_H

#include <libtcod.h>

typedef enum {
  GreaterThan,
  LessThan,
  EqualTo,
  GreaterThanOrEqualTo,
  LessThanOrEqualTo,
  
  StartsWith,
  EndsWith,
  FindString,
  IsString,
  
  CountGreaterThan,
  CountLessThan,
  CountEqualTo,
  CountGreaterThanOrEqualTo,
  CountLessThanOrEqualTo,
  
  ContainsNumber,
  ContainsString,
  ContainsAll
} ComparisonMode;

struct _comparison {
  ComparisonMode mode;
  TCOD_value_t value; //may be undefined, in which case pass the value
};
typedef struct _comparison *Comparison;

Comparison comparison_new();
Comparison comparison_init(Comparison c, ComparisonMode mode, TCOD_value_t val);
void comparison_free(Comparison c);

bool comparison_expects_rval(Comparison c);

bool comparison_result(Comparison c, TCOD_value_t lval, TCOD_value_t rval);


#endif