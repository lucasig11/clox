#ifndef CLOX_COMPILER_H
#define CLOX_COMPILER_H

#include "chunk.h"

bool compile(const char *src, Chunk *chunk);

#endif
