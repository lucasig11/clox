#ifndef CLOX_COMMON_H
#define CLOX_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG
#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE
#endif // DEBUG

#define UINT8_COUNT (UINT8_MAX + 1)

#endif // CLOX_COMMON_H
