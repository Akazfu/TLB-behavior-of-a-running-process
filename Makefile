CC = gcc
CFLAGS = -g -lm
RM = rm -f

MAIN = valtlb379	
TEST_PROG_1 = ./test_program/heapsort
TEST_PROG_2 = ./test_program/mergesort
TEST_PROG_3 = ./test_program/quicksort

SRC_MAIN = main.c
SRC_TLB  = tlb.c
SRC_TEST_PROG_1 = ./test_program/heapsort.c
SRC_TEST_PROG_2	= ./test_program/mergesort.c
SRC_TEST_PROG_3	= ./test_program/quicksort.c

OBJ_MAIN = $(SRC_MAIN:.c=.o)
OBJ_TLB = $(SRC_TLB:.c=.o)

all: $(MAIN) $(TEST_PROG_1) $(TEST_PROG_2) $(TEST_PROG_3)

$(MAIN): $(OBJ_MAIN) $(OBJ_TLB)
	$(CC) -o $(MAIN) $(OBJ_MAIN) $(OBJ_TLB) $(CFLAGS)

$(TEST_PROG_1): $(OBJ_TEST_PROG_1)
	$(CC) -o $(TEST_PROG_1) $(SRC_TEST_PROG_1)
$(TEST_PROG_2): $(OBJ_TEST_PROG_2)
	$(CC) -o $(TEST_PROG_2) $(SRC_TEST_PROG_2)
$(TEST_PROG_3): $(OBJ_TEST_PROG_3)
	$(CC) -o $(TEST_PROG_3) $(SRC_TEST_PROG_3)

test: test.o
	$(CC) -o test test.o

.PHONY: clean
clean: 
	$(RM) $(OBJ_MAIN) $(OBJ_TLB)