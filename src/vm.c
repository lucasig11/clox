#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include <stdio.h>

VM vm;

static void reset_stack() { vm.stack_top = vm.stack; }

void init_VM() { reset_stack(); }

void free_VM() {}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)                                                          \
  do {                                                                         \
    double rhs = pop();                                                        \
    double lhs = pop();                                                        \
    push(lhs op rhs);                                                          \
  } while (false)

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("        ");
    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
      printf("[");
      print_value(*slot);
      printf("]");
    }
    printf("\n");
    disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONST: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    case OP_ADD:
      BINARY_OP(+);
      break;
    case OP_SUB:
      BINARY_OP(-);
      break;
    case OP_MUL:
      BINARY_OP(*);
      break;
    case OP_DIV:
      BINARY_OP(/);
      break;
    case OP_NEGATE:
      push(-pop());
      break;
    case OP_RETURN: {
      print_value(pop());
      printf("\n");
      return INTERPRET_OK;
    }
    }
  }
#undef BINARY_OP
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(const char *source) {
  Chunk chunk;
  init_chunk(&chunk);

  if (!compile(source, &chunk)) {
    free_chunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  free_chunk(&chunk);

  return result;
}

void push(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop() {
  vm.stack_top--;
  return *vm.stack_top;
}
