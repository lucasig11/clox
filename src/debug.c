#include <stdio.h>

#include "../include/debug.h"

void disassemble_chunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->length;)
        offset = disassemble_instruction(chunk, offset);
}

static int simple_instruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset += 1;
}

static int const_instruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    print_value(chunk->constants.values[constant]);
    printf("'\n");
    return offset += 2;
}

int disassemble_instruction(Chunk *chunk, int offset) {
    printf("%04d ", offset);

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
    case OP_RETURN:
        return simple_instruction("OP_RETURN", offset);
    case OP_CONST:
        return const_instruction("OP_CONST", chunk, offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset += 1;
    }
}
