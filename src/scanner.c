#include <stdio.h>
#include <string.h>

#include "../include/common.h"
#include "../include/scanner.h"

Scanner scanner;

void init_scanner(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}
