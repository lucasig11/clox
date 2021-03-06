//! Macros and functions for memory manipulation.

#ifndef CLOX_MEMORY_H
#define CLOX_MEMORY_H

#include "common.h"
#include "object.h"

#define ALLOCATE(type, size) (type *)reallocate(NULL, 0, sizeof(type) * (size))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, old_len, new_len)                            \
  (type *)reallocate(pointer, sizeof(type) * old_len, sizeof(type) * new_len)

#define FREE_ARRAY(type, pointer, old_len)                                     \
  reallocate(pointer, sizeof(type) * old_len, 0)

void *reallocate(void *ptr, size_t old_size, size_t new_size);
void free_objects();
void collect_garbage();
void mark_object(Obj *object);
void mark_value(Value value);

#endif
