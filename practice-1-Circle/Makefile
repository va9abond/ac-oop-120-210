CXX      := clang++
CXXFLAGS := \
	-Werror \
	-pedantic-errors \
	-Wall \
	-Wextra \
	-O0 \
	-g3 \
	-std=c++20 \
	-Wno-unused-variable \
	-Wno-unused-macros \
	-Wno-unused-function \

# compile and run
default: main.cpp
	$(CXX) -o main main.cpp $(CXXFLAGS)
	command time ./main

test: test.cpp
	$(CXX) -o test test.cpp $(CXXFLAGS)
	./test
