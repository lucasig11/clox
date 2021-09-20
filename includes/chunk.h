//! A chunk of VM bytecode.
#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONST,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_NEGATE,
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
