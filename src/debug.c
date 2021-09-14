#include <stdio.h>

#include "../include/debug.h"

void disassemble_chunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->length;)
        disassemble_instruction(chunk, &offset);
}

static void simple_instruction(const char *name, int *offset) {
    printf("%s\n", name);
    *offset += 1;
}

static void const_instruction(const char *name, Chunk *chunk, int *offset) {
    uint8_t constant = chunk->code[*offset + 1];
    printf("'%-16s %4d '", name, constant);
    print_value(chunk->constants.values[constant]);
    printf("\n");
    *offset += 2;
}

void disassemble_instruction(Chunk *chunk, int *offset) {
    printf("%04d ", *offset);

    uint8_t instruction = chunk->code[*offset];
    switch (instruction) {
    case OP_RETURN:
        return simple_instruction("OP_RETURN", offset);
    case OP_CONST:
        return const_instruction("OP_CONST", chunk, offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        *offset += 1;
    }
}
