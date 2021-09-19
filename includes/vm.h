#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
  Chunk *chunk;
  // Instruction pointer
  uint8_t *ip;
  Value stack[STACK_MAX];
  // Address of the first empty slot in the stack
  Value *stack_top;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void init_VM();
void free_VM();
InterpretResult interpret(const char *);

// stack functions
void push(Value value);
Value pop();

#endif
