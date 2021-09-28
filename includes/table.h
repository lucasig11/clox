#ifndef CLOX_TABLE_H
#define CLOX_TABLE_H

#include "common.h"
#include "value.h"

typedef struct {
  // The key is always a string, so we can store the pointer directly
  // to the string object in the heap.
  ObjString *key;
  Value value;
} Entry;

typedef struct {
  // The ratio between `count` and `capacity` is the table's load factor.
  int count;
  int capacity;
  // Head of the entries list.
  Entry *entries;
} Table;

void init_table(Table *table);
void free_table(Table *table);
bool table_set(Table *table, ObjString *key, Value value);
// value is an output parameter
bool table_get(Table *table, ObjString *key, Value *value);
void table_copy(Table *src, Table *dest);

#endif
