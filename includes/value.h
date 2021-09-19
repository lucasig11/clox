#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"

typedef double Value;

typedef struct {
  int cap;
  int length;
  Value *values;
} ValueArray;

// Value functions
void print_value(Value);

// ValueArray functions
void init_value_array(ValueArray *);
void write_value_array(ValueArray *, Value);
void free_value_array(ValueArray *);

#endif
