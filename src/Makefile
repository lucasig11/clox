IDIR =../include
BDIR =../build/
LDIR =../lib
ODIR =obj

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

.PHONY: clean clox format

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CC_FLAGS)

bin: $(OBJ)
	$(CC) -o $(BDIR)/$(BIN) $^ $(CC_FLAGS) $(LIBS)

format:
	clang-format --style='{IndentWidth: 4}' -i *.c $(IDIR)/*.h

clox: format bin

clean:
	$(RM) $(ODIR)/*.o $(BDIR)/*
