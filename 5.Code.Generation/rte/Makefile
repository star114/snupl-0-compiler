CC=gcc
CCFLAGS=-m32 -std=c99 -O1

RTE_SRC=IO.s
TEST_SRC=test_io.c

RTE_OBJ=$(patsubst %.s,%.o,$(RTE_SRC))
TEST_OBJ=$(patsubst %.c,%.o,$(TEST_SRC))

TEST_EXE=test_io

.PHONY: clean test

%.o: %.s
	$(CC) $(CCFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

rte: $(RTE_OBJ)

test: rte $(TEST_OBJ)
	$(CC) $(CCFLAGS) -o $(TEST_EXE) $(TEST_OBJ) $(RTE_OBJ)

clean:
	rm -rf *.o $(TEST_EXE)
