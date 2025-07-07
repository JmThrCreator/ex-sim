CC = gcc
CFLAGS = -Wall -Werror -Wextra -I$(PWD)/lib -lm

SRC = ex_sim.c
OUT = bin/ex_sim

all: $(OUT)
	./$(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

TEST_DIR = tests
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_OUT = $(patsubst $(TEST_DIR)/%.c,%,$(TEST_SRC))
TEST_CFLAGS = -Wall -Werror -Wextra -I$(PWD) -I$(PWD)/lib/munit -DTESTING lib/munit/munit.c $(SRC)

tests: $(TEST_OUT)

%: $(TEST_DIR)/%.c
	$(CC) $(TEST_CFLAGS) -o $(TEST_OUT) $(TEST_SRC)

test: tests
	@for bin in $(TEST_OUT); do \
		./$$bin; \
	done

clean:
	rm -f $(OUT) $(TEST_OUT)

.PHONY: all clean
