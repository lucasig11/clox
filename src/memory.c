#include <stdlib.h>

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include "debug.h"
#include <stdio.h>
#endif

#define GC_HEAP_GROW_FACTOR 2

void *reallocate(void *ptr, size_t old_size, size_t new_size) {
  vm.bytes_allocated += new_size - old_size;
  if (new_size > old_size) {
#ifdef DEBUG_STRESS_GC
    collect_garbage();
#endif

    if (vm.bytes_allocated >= vm.next_gc) {
      collect_garbage();
    }
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
  if (object == NULL || is_marked(object))
    return;

#ifdef DEBUG_LOG_GC
  printf("[GC] Mark -> %p\n", (void *)object);
  print_value(OBJ_VAL(object));
  printf("\n");
#endif
  obj_set_mark(object, true);

  if (vm.gray_cap < vm.gray_count + 1) {
    vm.gray_cap = GROW_CAPACITY(vm.gray_cap);
    vm.gray_stack = realloc(vm.gray_stack, vm.gray_cap * sizeof(Obj *));
    if (vm.gray_stack == NULL)
      exit(1);
  }

  vm.gray_stack[vm.gray_count++] = object;
}

void mark_value(Value value) {
  if (IS_OBJ(value)) {
    mark_object(AS_OBJ(value));
  }
}

void mark_array(ValueArray *array) {
  for (int i = 0; i < array->length; i++) {
    mark_value(array->values[i]);
  }
}

void blacken_object(Obj *object) {
#ifdef DEBUG_LOG_GC
  printf("[GC] Blacken -> %p ", (void *)object);
  print_value(OBJ_VAL(object));
  printf("\n");
#endif
  switch (obj_type(object)) {
  case OBJ_CLASS: {
    ObjClass *klass = (ObjClass *)object;
    mark_object((Obj *)klass->name);
    break;
  }
  case OBJ_CLOSURE: {
    ObjClosure *closure = (ObjClosure *)object;
    mark_object((Obj *)closure->function);
    for (int i = 0; i < closure->upvalue_count; i++) {
      mark_object((Obj *)closure->upvalues[i]);
    }
    break;
  }
  case OBJ_FUNCTION: {
    ObjFunction *function = (ObjFunction *)object;
    mark_object((Obj *)function->name);
    mark_array(&function->chunk.constants);
    break;
  }
  case OBJ_UPVALUE:
    mark_value(((ObjUpvalue *)object)->closed);
    break;
  case OBJ_NATIVE:
  case OBJ_STRING:
    break;
  }
}

static void free_object(Obj *object) {
#ifdef DEBUG_LOG_GC
  printf("[GC] Drop -> %p (%d)\n", (void *)object, obj_type(object));
#endif
  switch (obj_type(object)) {
  case OBJ_CLASS: {
    FREE(ObjClass, object);
    break;
  }
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

static void trace_references() {
  while (vm.gray_count > 0) {
    Obj *object = vm.gray_stack[--vm.gray_count];
    blacken_object(object);
  }
}

static void sweep() {
  Obj **object = &vm.objects;

  while ((*object)) {
    if (is_marked(*object)) {
      obj_set_mark(*object, false);
      *object = obj_next(*object);
    } else {
#ifdef DEBUG_LOG_GC
      printf("[GC] Sweep -> %p (%d) ", (void *)*object, obj_type(*object));
      print_value(OBJ_VAL(*object));
      printf("\n");
#endif
      Obj *unreached = *object;
      *object = obj_next(unreached);
      free_object(unreached);
    }
  }
}

void free_objects() {
  Obj *object = vm.objects;
  while (object != NULL) {
    Obj *next = obj_next(object);
    free_object(object);
    object = next;
  }
  free(vm.gray_stack);
}

void collect_garbage() {
#ifdef DEBUG_LOG_GC
  printf("[GC] Start\n");
  size_t before = vm.bytes_allocated;
#endif

  mark_roots();
  trace_references();
  table_remove_white(&vm.strings);
  sweep();

  vm.next_gc = vm.bytes_allocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
  printf("[GC] End\n");
  printf("[GC] Collected %zu bytes (from %zu to %zu) next at %zu\n",
         before - vm.bytes_allocated, before, vm.bytes_allocated, vm.next_gc);
#endif
}
