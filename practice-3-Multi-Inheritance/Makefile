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

CXXFLAGS_RELEASE := \
	-Werror \
	-pedantic-errors \
	-Wall \
	-Wextra \
	-O0 \
	-g3 \
	-std=c++20 \

# compile and run
default: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp
	command time ./main

release: main.cpp
	$(CXX) $(CXXFLAGS_RELEASE) -o release main.cpp
	./release

test: test.cpp
	$(CXX) $(CXXFLAGS) -o test test.cpp
	./test


