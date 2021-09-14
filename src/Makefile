IDIR =../include
ODIR =obj
LDIR =../lib

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
		common.h

_OBJ = main.o \
	   chunk.o \
	   memory.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CC_FLAGS)

clox: $(OBJ)
		$(CC) -o ../build/$@ $^ $(CC_FLAGS) $(LIBS)

.PHONY: clean clox

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
