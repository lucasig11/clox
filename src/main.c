#include "../include/common.h"
#include "../include/chunk.h"

int main(int argc, char *argv[]) { 
	Chunk chunk;
	init_chunk(&chunk);
	write_chunk(&chunk, OP_RETURN);
	free_chunk(&chunk);
	return 0;
}
