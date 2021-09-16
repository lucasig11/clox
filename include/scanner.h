#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

typedef struct {
    const char *start;
    const char *current;
    int line;
} Scanner;

void init_scanner();

#endif
