SRC_PATH=./src
TEST_PATH=./test

all:
	make -C $(SRC_PATH)

.PHONY: test
test:
	make -C $(TEST_PATH)

run-tests: test
	make -C $(TEST_PATH) run-tests

clean:
	make -C $(SRC_PATH) clean
	make -C $(TEST_PATH) clean
