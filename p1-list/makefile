CC = g++
CXX_FLAGS = -Wall -Wextra -pedantic
SAN_FLAGS = -fsanitize=address \
			-fsanitize=undefined \
			-fsanitize=leak \
			-fno-omit-frame-pointer
# -fsanitize=memory \


main: main.cpp
	$(CC) $(CXX_FLAGS) $(SAN_FLAGS) -o main main.cpp

run: main
	./main

.PHONY: run
