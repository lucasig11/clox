#include <stdio.h>

#include "memory.h"
#include "value.h"

void print_value(Value value) {
  switch (value.type) {
  case VAL_BOOL:
    printf(AS_BOOL(value) ? "true" : "false");
    break;
  case VAL_NIL:
    printf("nil");
    break;
  case VAL_NUMBER:
    printf("%g", AS_NUMBER(value));
    break;
  }
}

void init_value_array(ValueArray *array) {
  array->values = NULL;
  array->cap = 0;
  array->length = 0;
}

void write_value_array(ValueArray *array, Value value) {
  if (array->cap < array->length + 1) {
    int old_cap = array->cap;

    array->cap = GROW_CAPACITY(old_cap);
    array->values = GROW_ARRAY(Value, array->values, old_cap, array->cap);
  }

  array->values[array->length] = value;
  array->length++;
}

void free_value_array(ValueArray *array) {
  FREE_ARRAY(Value, array->values, array->cap);
  init_value_array(array);
}
