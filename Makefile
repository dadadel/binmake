SRC_PATH=./src
TEST_PATH=./test

all:
	make -C $(SRC_PATH)

.PHONY: test
test:
	make -C $(TEST_PATH)


clean:
	make -C $(SRC_PATH) clean
	make -C $(TEST_PATH) clean
