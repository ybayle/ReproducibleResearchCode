# BIN = /home/tl/bin/
BIN = ..
# O = /home/tl/obj
O = .
F = -g -O3 -ansi -pedantic
CC = gcc
GCC_EXEC_PREFIX=""

#----------------------------------------------------------------------
src = main.c matrix.c iof.c error.c matrand.c random.c numerica2.c string-utils.c

vqmm: $(patsubst %.c,$(O)/%.o,$(patsubst %.cc,$(O)/%.o,$(src)))
	$(CC) $(F) -o $(BIN)/vqmm $^ -lm

clean:
	rm -f *.o $(BIN)/vqmm 

# TODO: include correct flags in order to optimize code.
