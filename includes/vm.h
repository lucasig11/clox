#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct{
  ObjClosure *closure;
  uint8_t *ip;
  Value *slots;
} CallFrame;

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frame_count;
  // Vm's stack
  Value stack[STACK_MAX];
  // Address of the first empty slot in the stack
  Value *stack_top;
  // Intern pool of string literals
  Table strings;
  // Global variables pool
  Table globals;
  // Pointer to the head of the objects list (for gc)
  Obj *objects;
  // Pointer to the head of the upvalues list
  ObjUpvalue *open_upvalues;

  // Grey GC Objects
  int gray_count;
  int gray_cap;
  Obj **gray_stack;

  // Bytes allocated by the VM
  size_t bytes_allocated;
  // Next `bytes_allocated` threshold to run the GC
  size_t next_gc;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;

void init_VM();
void free_VM();
InterpretResult interpret(const char *);

// stack functions
void push(Value value);
Value pop();

#endif
