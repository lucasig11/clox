#include "../include/chunk.h"
#include "../include/memory.h"
#include "../include/value.h"
#include <stdlib.h>

void init_chunk(Chunk *chunk) {
    chunk->length = 0;
    chunk->cap = 0;
    chunk->code = NULL;
    init_value_array(&chunk->constants);
}

void free_chunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->cap);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

void write_chunk(Chunk *chunk, uint8_t byte) {
    if (chunk->cap < chunk->length + 1) {
        int old_cap = chunk->cap;
        chunk->cap = GROW_CAPACITY(old_cap);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, old_cap, chunk->cap);
    }

    chunk->code[chunk->length] = byte;
    chunk->length++;
}

int add_constant(Chunk *chunk, Value value) {
    write_value_array(&chunk->constants, value);
    return chunk->constants.length - 1;
}
