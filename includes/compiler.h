#ifndef CLOX_COMPILER_H
#define CLOX_COMPILER_H

#include "chunk.h"
#include "object.h"

ObjFunction *compile(const char *src);
void mark_compiler_roots();

#endif
