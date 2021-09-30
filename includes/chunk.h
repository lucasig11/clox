//! A chunk of VM bytecode.
#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
  // Loads a constant from the chunk's constant pool.
  OP_CONST,
  // Loads a `nil` constant to the VM stack.
  OP_NIL,
  // Loads a `true` constant to the VM stack.
  OP_TRUE,
  // Loads a `false` constant to the VM stack.
  OP_FALSE,
  // Consumes the top values from the VM stack.
  OP_POP,
  // Consumes `n` top values from the VM stack.
  OP_POPN,
  OP_GET_LOCAL,
  OP_SET_LOCAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL,
  OP_DEFINE_GLOBAL,
  // Comparison operations (compares the two first values in the stack)
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  // Pops the two uppermost values from the stack as operands and pushes the
  // result.
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  // Logically negate the value at the top of the VM's stack.
  OP_NOT,
  // Arithmetically negate the value at the top of the VM's stack.
  OP_NEGATE,
  OP_PRINT,
  OP_RETURN
} OpCode;

// Dynamic array to store a sequence of OpCodes
typedef struct {
  int length;
  int cap;
  int *lines;
  uint8_t *code;
  ValueArray constants;
} Chunk;

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
void write_chunk(Chunk *chunk, uint8_t byte, int lines);

int add_constant(Chunk *chunk, Value value);

#endif
