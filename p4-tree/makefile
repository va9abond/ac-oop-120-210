CC = g++
CXX_FLAGS = -Wall -Wextra -pedantic-errors --std=c++20 -O0
SAN_FLAGS = -fsanitize=address \
			-fsanitize=undefined \
			-fsanitize=leak \
			-fno-omit-frame-pointer
# -fsanitize=memory \

TARGET = tree
SUPPRESSOR = -Wno-unused-parameter -Wno-unused-variable


build: $(TARGET)

$(TARGET): tree.o
	@$(CC) tree.o -o $(TARGET)

tree.o: tree.cpp
	@$(CC) -c $(CXX_FLAGS) $(SUPPRESSOR) -o tree.o tree.cpp

valgrind: $(TARGET)
	@valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm tree.o
	rm $(TARGET)

run: $(TARGET)
	./$(TARGET)
	

.PHONY: run valgrind build clean

