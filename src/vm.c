#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

VM vm;

#define CHECK_ARITY(_count, _arity)                                            \
  if (_count != _arity) {                                                      \
    runtime_error("Expected %d arguments but got %d.", _arity, _count);        \
    return false;                                                              \
  }

static Value clockNative(int argc, Value *argv) {
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

static void reset_stack() {
  vm.stack_top = vm.stack;
  vm.frame_count = 0;
}

static void runtime_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fputs("\n", stderr);
  // Print the stack trace
  for (int i = vm.frame_count - 1; i >= 0; --i) {
    CallFrame *frame = &vm.frames[i];
    ObjFunction *function = frame->function;
    size_t instruction = frame->ip - function->chunk.code - 1;
    fprintf(stderr, "[line %d] in ", function->chunk.lines[instruction]);
    if (function->name == NULL)
      fprintf(stderr, "script\n");
    else
      fprintf(stderr, "%s()\n", function->name->chars);
  }
  reset_stack();
}

static void define_native(const char *name, NativeFn function, uint8_t arity) {
  push(OBJ_VAL(copy_string(name, strlen(name))));
  push(OBJ_VAL(new_native(function, arity)));
  table_set(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1]);
  pop();
  pop();
}

void init_VM() {
  reset_stack();
  vm.objects = NULL;
  init_table(&vm.globals);
  init_table(&vm.strings);

  define_native("clock", clockNative, 0);
}

void free_VM() {
  free_table(&vm.globals);
  free_table(&vm.strings);
  free_objects();
}

static Value peek(int distance) { return vm.stack_top[-1 - distance]; }

static bool call(ObjFunction *function, int argc) {
  if (argc != function->arity) {
    runtime_error("Expected %d arguments but got %d.", function->arity, argc);
    return false;
  }

  if (vm.frame_count == FRAMES_MAX) {
    runtime_error("Stack overflow");
    return false;
  }

  CallFrame *frame = &vm.frames[vm.frame_count++];
  frame->function = function;
  frame->ip = function->chunk.code;
  frame->slots = vm.stack_top - argc - 1;
  return true;
}

static bool call_value(Value callee, int argc) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
    case OBJ_FUNCTION:
      return call(AS_FUNCTION(callee), argc);
    case OBJ_NATIVE: {
      uint8_t arity = ((ObjNative *)AS_OBJ(callee))->arity;
      CHECK_ARITY(argc, arity)
      NativeFn native = AS_NATIVE(callee);
      Value result = native(argc, vm.stack_top - argc);
      vm.stack_top -= argc + 1;
      push(result);
      return true;
    }
    default:
      break; // Non-callable object type
    }
  }
  runtime_error("Can only call functions and classes.");
  return false;
}

static bool is_falsy(Value value) {
  // `nil` and `false` are falsy. Everything else is truthy.
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
  ObjString *rhs = AS_STRING(pop());
  ObjString *lhs = AS_STRING(pop());

  int length = lhs->length + rhs->length;
  char *chars = ALLOCATE(char, length + 1);
  memcpy(chars, lhs->chars, lhs->length);
  memcpy(chars + lhs->length, rhs->chars, rhs->length);
  chars[length] = '\0';

  ObjString *result = take_string(chars, length);
  push(OBJ_VAL(result));
}

static InterpretResult run() {
  CallFrame *frame = &vm.frames[vm.frame_count - 1];
  register uint8_t *ip = frame->ip;

#define READ_BYTE() (*ip++)

// Builds a 16-bit uint from the next two bytes in the chunk
#define READ_SHORT() (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))

#define READ_CONSTANT() (frame->function->chunk.constants.values[READ_BYTE()])

#define READ_STRING() AS_STRING(READ_CONSTANT())

#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {                          \
      frame->ip = ip;                                                          \
      runtime_error("Operands must be numbers.");                              \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double rhs = AS_NUMBER(pop());                                             \
    double lhs = AS_NUMBER(pop());                                             \
    push(valueType(lhs op rhs));                                               \
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
    disassemble_instruction(&frame->function->chunk,
                            (int)(ip - frame->function->chunk.code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_CONST: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    case OP_NIL:
      push(NIL_VAL);
      break;
    case OP_TRUE:
      push(BOOL_VAL(true));
      break;
    case OP_FALSE:
      push(BOOL_VAL(false));
      break;
    case OP_POP:
      pop();
      break;
    case OP_POPN:
      vm.stack_top -= READ_BYTE();
      break;
    case OP_GET_LOCAL: {
      uint8_t slot = READ_BYTE();
      push(frame->slots[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = READ_BYTE();
      frame->slots[slot] = peek(0);
      break;
    }
    case OP_DEFINE_GLOBAL: {
      ObjString *name = READ_STRING();
      table_set(&vm.globals, name, peek(0));
      pop();
      break;
    }
    case OP_GET_GLOBAL: {
      ObjString *name = READ_STRING();
      Value value;
      if (!table_get(&vm.globals, name, &value)) {
        frame->ip = ip;
        runtime_error("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      push(value);
      break;
    }
    case OP_SET_GLOBAL: {
      ObjString *name = READ_STRING();
      if (table_set(&vm.globals, name, peek(0))) {
        table_delete(&vm.globals, name);
        frame->ip = ip;
        runtime_error("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_EQUAL: {
      Value lhs = pop();
      Value rhs = pop();
      push(BOOL_VAL(values_eq(rhs, lhs)));
      break;
    }
    case OP_GREATER:
      BINARY_OP(BOOL_VAL, >);
      break;
    case OP_LESS:
      BINARY_OP(BOOL_VAL, <);
      break;
    case OP_ADD: {
      if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
        concatenate();
      } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
        double rhs = AS_NUMBER(pop());
        double lhs = AS_NUMBER(pop());
        push(NUMBER_VAL(lhs + rhs));
      } else {
        frame->ip = ip;
        runtime_error("Operands must be two numbers or two strings.");
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_SUB:
      BINARY_OP(NUMBER_VAL, -);
      break;
    case OP_MUL:
      BINARY_OP(NUMBER_VAL, *);
      break;
    case OP_DIV:
      BINARY_OP(NUMBER_VAL, /);
      break;
    case OP_NOT:
      push(BOOL_VAL(is_falsy(pop())));
      break;
    case OP_NEGATE: {
      // Check if stack_top is an integer
      if (!IS_NUMBER(peek(0))) {
        frame->ip = ip;
        runtime_error("Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      push(NUMBER_VAL(-AS_NUMBER(pop())));
      break;
    }
    case OP_PRINT: {
      print_value(pop());
      printf("\n");
      break;
    }
    case OP_JUMP_IF_FALSE: {
      uint16_t offset = READ_SHORT();
      if (is_falsy(peek(0)))
        ip += offset;
      break;
    }
    case OP_JUMP_IF_TRUE: {
      uint16_t offset = READ_SHORT();
      if (!is_falsy(peek(0)))
        ip += offset;
      break;
    }
    case OP_JUMP: {
      uint16_t offset = READ_SHORT();
      ip += offset;
      break;
    }
    case OP_LOOP: {
      uint16_t offset = READ_SHORT();
      ip -= offset;
      break;
    }
    case OP_CALL: {
      int argc = READ_BYTE();
      frame->ip = ip;
      if (!call_value(peek(argc), argc)) {
        return INTERPRET_RUNTIME_ERROR;
      }
      frame = &vm.frames[vm.frame_count - 1];
      ip = frame->ip;
      break;
    }
    case OP_RETURN: {
      // Get the result from the stack
      Value result = pop();
      vm.frame_count--;

      // Top-level function. Exit the interpreter.
      if (vm.frame_count == 0) {
        pop();
        return INTERPRET_OK;
      }

      // Drop function arguments slots
      vm.stack_top = frame->slots;
      // Push the return value back to the stack
      push(result);
      // Update the cached frame
      frame = &vm.frames[vm.frame_count - 1];
      ip = frame->ip;
      break;
    }
    }
  }
#undef BINARY_OP
#undef READ_BYTE
#undef READ_STRING
#undef READ_SHORT
#undef READ_CONSTANT
}

InterpretResult interpret(const char *source) {
  ObjFunction *function = compile(source);

  if (function == NULL)
    return INTERPRET_COMPILE_ERROR;

  push(OBJ_VAL(function));

  call(function, 0);

  return run();
}

void push(Value value) {
  *vm.stack_top = value;
  vm.stack_top++;
}

Value pop() {
  vm.stack_top--;
  return *vm.stack_top;
}
