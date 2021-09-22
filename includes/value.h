#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "common.h"

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
  } as;
} Value;

// Macros to cast LoxValues to C types
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

// Macros to verify LoxValue type
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

// Macros to create LoxValues from C types
#define BOOL_VAL(value) 	((Value) {VAL_BOOL, {.boolean = value}})
#define NUMBER_VAL(value) 	((Value) {VAL_NUMBER, {.number = value}})
#define NIL_VAL 			((Value) {VAL_NIL, {.number = 0}})

typedef struct {
  int cap;
  int length;
  Value *values;
} ValueArray;

// Value functions
void print_value(Value);
bool values_eq(Value, Value);

// ValueArray functions
void init_value_array(ValueArray *);
void write_value_array(ValueArray *, Value);
void free_value_array(ValueArray *);

#endif
