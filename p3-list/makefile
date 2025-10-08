CC = g++
CXX_FLAGS = -Wall -Wextra -pedantic
SAN_FLAGS = -fsanitize=address \
			-fsanitize=undefined \
			-fsanitize=leak \
			-fno-omit-frame-pointer
# -fsanitize=memory \

list: list.cpp
	$(CC) $(CXX_FLAGS) -O0 -g -o list list.cpp

compile: list.cpp
	$(CC) $(CXX_FLAGS) -O0 -g $(SAN_FLAGS) -o list list.cpp

run: list
	./list

valgrind: list
	valgrind --leak-check=full --show-leak-kinds=all ./list

.PHONY: run valgrind
