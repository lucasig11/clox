#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

#include "chunk.h"
#include "common.h"
#include "value.h"

// Extracts the type from an object.
#define OBJ_TYPE(value) (obj_type(AS_OBJ(value)))

#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function)
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
  OBJ_CLOSURE,
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_STRING,
  OBJ_UPVALUE,
} ObjType;

struct Obj {
  // 64-bit (8 bytes) integer header:
  // 1 byte (mark) | 6 bytes (next) | 1 byte (type)
  uint64_t header;
};

typedef struct {
  Obj obj;
  int arity;
  int upvalue_count;
  Chunk chunk;
  ObjString *name;
} ObjFunction;

typedef Value (*NativeFn)(int argc, Value *argv);

typedef struct {
  Obj obj;
  NativeFn function;
  uint8_t arity;
} ObjNative;

struct ObjString {
  Obj obj;
  int length;
  char *chars;
  uint32_t hash;
};

typedef struct ObjUpvalue {
  Obj obj;
  // Pointer to the closed-over variable
  Value *location;
  // Pointer to the next upvalue (list)
  struct ObjUpvalue *next;
  Value closed;
} ObjUpvalue;

typedef struct {
  Obj obj;
  ObjFunction *function;
  ObjUpvalue **upvalues;
  int upvalue_count;
} ObjClosure;

ObjClosure *new_closure(ObjFunction *function);
ObjFunction *new_function();
ObjNative *new_native(NativeFn function, uint8_t arity);
ObjString *take_string(char *chars, int length);
ObjString *copy_string(const char *chars, int length);
ObjUpvalue *new_upvalue(Value *slot);
void print_object(Value value);

inline ObjType obj_type(Obj *object) {
  // The type is stored in the lowest 3 bits of the header (0111 = 0x07).
  return (ObjType)(object->header & 0x07);
}

inline bool is_marked(Obj *object) {
  // The mark bit is the most significant bit in the header.
  return ((object->header >> 56) & 0x01);
}

inline Obj *obj_next(Obj *object) {
  // The next pointer is stored in the 6 bytes at the middle of the header,
  // between the mark bit and the type.
  // Shift the header 1 byte to the right and mask off the the mark bit.
  return (Obj *)((object->header >> 8) & 0x00FFFFFFFFFFFFFF);
}

inline void obj_set_mark(Obj *object, bool is_marked) {
  // Set the mark bit in the header.
  object->header =
      (object->header & 0x00FFFFFFFFFFFFFF) | ((uint64_t)is_marked << 56);
}

inline void obj_set_next(Obj *object, Obj *next) {
  // Set the next pointer in the header.
  object->header =
      (object->header & 0xFF000000000000FF) | ((uint64_t)next << 8);
}

static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && obj_type(AS_OBJ(value)) == type;
}

#endif
