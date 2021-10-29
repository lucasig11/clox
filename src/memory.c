#include <stdlib.h>

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include "debug.h"
#include <stdio.h>
#endif

void *reallocate(void *ptr, size_t old_size, size_t new_size) {
  if (new_size > old_size) {
#ifdef DEBUG_STRESS_GC
    collect_garbage();
#endif
  }
  if (new_size == 0) {
    free(ptr);
    return NULL;
  }

  void *result = realloc(ptr, new_size);

  if (result == NULL)
    exit(1);

  return result;
}

void mark_object(Obj *object) {
  if (object == NULL)
    return;

#ifdef DEBUG_LOG_GC
  printf("[GC] Mark -> %p\n", (void *)object);
  print_value(OBJ_VAL(object));
  printf("\n");
#endif
  object->marked = true;
}

void mark_value(Value value) {
  if (IS_OBJ(value)) {
    mark_object(AS_OBJ(value);
  }
}

static void free_object(Obj *object) {
#ifdef DEBUG_LOG_GC
  printf("[GC] Drop -> %p (%d)\n", (void *)object, object->type);
#endif
  switch (object->type) {
  case OBJ_CLOSURE: {
    ObjClosure *closure = (ObjClosure *)object;
    FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalue_count);
    FREE(ObjClosure, object);
    break;
  }
  case OBJ_FUNCTION: {
    ObjFunction *function = (ObjFunction *)object;
    free_chunk(&function->chunk);
    FREE(ObjFunction, object);
    break;
  }
  case OBJ_NATIVE: {
    FREE(ObjNative, object);
    break;
  }
  case OBJ_STRING: {
    ObjString *string = (ObjString *)object;
    FREE_ARRAY(char, string->chars, string->length + 1);
    FREE(ObjString, object);
    break;
  }
  case OBJ_UPVALUE: {
    FREE(ObjUpvalue, object);
    break;
  }
  }
}

static void mark_roots() {
  for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
    mark_value(*slot);
  }

  for (int i = 0; i < vm.frame_count; i++) {
    mark_object((Obj *)&vm.frames[i].closure);
  }

  for (ObjUpvalue *upvalue = vm.open_upvalues; upvalue != NULL; upvalue++) {
    mark_object((Obj *)upvalue);
  }

  mark_table(&vm.globals);
  mark_compiler_roots();
}

void free_objects() {
  Obj *object = vm.objects;
  while (object != NULL) {
    Obj *next = object->next;
    free_object(object);
    object = next;
  }
}

void collect_garbage() {
#ifdef DEBUG_LOG_GC
  printf("[GC] Start\n");
#endif

  mark_roots();

#ifdef DEBUG_LOG_GC
  printf("[GC] End\n");
#endif
}
