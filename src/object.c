#include <stdio.h>
#include <string.h>

#include "chunk.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, obj_type)                                           \
  (type *)allocate_object(sizeof(type), obj_type)

static Obj *allocate_object(size_t size, ObjType type) {
  Obj *object = (Obj *)reallocate(NULL, 0, size);
  object->header = (unsigned long)type | (unsigned long)vm.objects << 8;
  vm.objects = object;

#ifdef DEBUG_LOG_GC
  printf("[GC] Allocate -> %p (%d) %zu bytes\n", (void *)object,
         obj_type(object), size);
#endif
  return object;
}

ObjClosure *new_closure(ObjFunction *function) {
  ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue *, function->upvalue_count);

  for (int i = 0; i < function->upvalue_count; i++) {
    upvalues[i] = NULL;
  }

  ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
  closure->function = function;
  closure->upvalues = upvalues;
  closure->upvalue_count = function->upvalue_count;

  return closure;
}

ObjFunction *new_function() {
  ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
  function->arity = 0;
  function->upvalue_count = 0;
  function->name = NULL;
  init_chunk(&function->chunk);
  return function;
}

ObjNative *new_native(NativeFn function, uint8_t arity) {
  ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  native->arity = arity;
  return native;
}

static ObjString *allocate_string(char *chars, int length, uint32_t hash) {
  ObjString *str = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  ObjString *interned = table_find_string(&vm.strings, chars, length, hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }
  str->length = length;
  str->chars = chars;
  str->hash = hash;
  push(OBJ_VAL(str));
  table_set(&vm.strings, str, NIL_VAL);
  pop();
  return str;
}

// FNV-1a (Fowler-Noll-Vo) hashing function.
static uint32_t hash_str(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString *take_string(char *chars, int length) {
  uint32_t hash = hash_str(chars, length);
  return allocate_string(chars, length, hash);
}

ObjString *copy_string(const char *chars, int length) {
  uint32_t hash = hash_str(chars, length);
  ObjString *interned = table_find_string(&vm.strings, chars, length, hash);
  if (interned != NULL)
    return interned;
  char *heap_chars = ALLOCATE(char, length + 1);
  memcpy(heap_chars, chars, length);
  heap_chars[length] = '\0';
  return allocate_string(heap_chars, length, hash);
}

ObjUpvalue *new_upvalue(Value *slot) {
  ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
  upvalue->location = slot;
  upvalue->next = NULL;
  upvalue->closed = NIL_VAL;
  return upvalue;
}

static void print_function(ObjFunction *function) {
  if (function->name == NULL) {
    printf("<script>");
    return;
  }
  printf("<fn %s>", function->name->chars);
}

void print_object(Value value) {
  switch
    OBJ_TYPE(value) {
    case OBJ_CLOSURE: {
      print_function(AS_CLOSURE(value)->function);
      break;
    }
    case OBJ_FUNCTION:
      print_function(AS_FUNCTION(value));
      break;
    case OBJ_NATIVE:
      printf("<native fn>");
      break;
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
    case OBJ_UPVALUE:
      printf("upvalue");
      break;
    }
}
