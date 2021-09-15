IDIR =../include
BDIR =../build
LDIR =../lib
ODIR =obj

MK_DIR=mkdir -p
CC=clang
CC_FLAGS =-I $(IDIR)
CC_FLAGS += -ggdb3 \
		-O2	\
		-Qunused-arguments \
		-std=c11 \
		-Wall \
		-Werror \
		-Wextra \
		-Wno-sign-compare \
		-Wno-unused-parameter \
		-Wno-unused-variable \
		-Wshadow

LIBS=

_DEPS = chunk.h \
		memory.h \
		vm.h \
		debug.h \
		value.h \
		common.h

_OBJ = main.o \
	   chunk.o \
	   memory.o \
	   vm.o \
	   debug.o \
	   value.o

BIN=clox
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: clean clox format directories

# Compile obj files
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CC_FLAGS)

# Compile binary
bin: $(OBJ)
	$(CC) -o $(BDIR)/$(BIN) $^ $(CC_FLAGS) $(LIBS)

# Create build and obj directories
directories:
	$(MK_DIR) $(BDIR) $(ODIR)

# Formats files in src and include directories
format:
	clang-format --style='{IndentWidth: 4}' -i *.c $(IDIR)/*.h

clox: format directories bin

# Cleans obj and build directories
clean:
	$(RM) $(ODIR)/*.o $(BDIR)/*
