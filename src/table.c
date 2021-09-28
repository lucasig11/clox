#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

void init_table(Table *table) {
  table->capacity = 0;
  table->count = 0;
  table->entries = NULL;
}

void free_table(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);

  init_table(table);
}

static Entry *find_entry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index = key->hash % capacity;
  Entry *tombstone = NULL;

  while (true) {
    Entry *entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        // Empty entry, return the tombstone as an available bucket
        return tombstone != NULL ? tombstone : entry;
      } else {
        // Here we know that `entry` is a tombstone
        if (tombstone == NULL)
          tombstone = entry;
      }
    } else if (entry->key == key) {
      return entry;
    }
    index = (index + 1) % capacity;
  }
}

static void adjust_capacity(Table *table, int capacity) {
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  table->count = 0;
  // Copy contents of the old table to the new one
  // considering the new capacity for the hash function.
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL)
      continue;

    Entry *dest = find_entry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }

  // Free the old list
  FREE_ARRAY(Entry, table->entries, table->capacity);

  // Update the table pointers
  table->entries = entries;
  table->capacity = capacity;
}

bool table_set(Table *table, ObjString *key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjust_capacity(table, capacity);
  }

  Entry *entry = find_entry(table->entries, table->capacity, key);
  bool is_new_key = entry->key == NULL;
  if (is_new_key && IS_NIL(entry->value))
    table->count++;

  entry->key = key;
  entry->value = value;

  return is_new_key;
}

bool table_get(Table *table, ObjString *key, Value *value) {
  if (table->count == 0)
    return false;

  Entry *entry = find_entry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  *value = entry->value;
  return true;
}

bool table_delete(Table *table, ObjString *key, Value *value) {
  if (table->count == 0)
    return false;

  Entry *entry = find_entry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  // Mark as a tombstone
  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}

void table_copy(Table *src, Table *dest) {
  for (int i = 0; i < src->capacity; i++) {
    Entry *entry = &src->entries[i];
    if (entry->key == NULL)
      continue;
    table_set(dest, entry->key, entry->value);
  }
}

ObjString *table_find_string(Table *table, const char *chars, int length,
                             uint32_t hash) {
  if (table->count == 0)
    return NULL;

  uint32_t index = hash % table->capacity;

  while (true) {
    Entry *entry = &table->entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value))
        return NULL;
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      return entry->key;
    }
    index = (index + 1) % table->capacity;
  }
}
