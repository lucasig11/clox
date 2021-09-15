#include "../include/vm.h"
#include "../include/debug.h"
#include "../include/value.h"
#include <stdio.h>

VM vm;

void init_VM() {}

void free_VM() {}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case OP_CONST: {
            Value constant = READ_CONSTANT();
            print_value(constant);
            printf("\n");
            break;
        }
        case OP_RETURN:
            return INTERPRET_OK;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk *chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
