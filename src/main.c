#include "../include/chunk.h"
#include "../include/common.h"
#include "../include/debug.h"

int main(int argc, char *argv[]) {
    Chunk chunk;
    init_chunk(&chunk);
    write_chunk(&chunk, OP_RETURN);
    int constant = add_constant(&chunk, 1.2);
    write_chunk(&chunk, OP_CONST);
    write_chunk(&chunk, constant);
    disassemble_chunk(&chunk, "test chunk");
    free_chunk(&chunk);
    return 0;
}
