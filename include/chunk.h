#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "common.h"

typedef enum { OP_RETURN } OpCode;

// Dynamic array to store a sequence of OpCodes
typedef struct {
    int length;
    int cap;
    uint8_t *code;
} Chunk;

void init_chunk(Chunk *chunk);
void free_chunk(Chunk *chunk);
// Append byte to chunk
void write_chunk(Chunk *chunk, uint8_t byte);

#endif
