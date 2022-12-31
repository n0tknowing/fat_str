# this makefile is just for development
CXX=clang++-14
CXXFLAGS=-std=c++14 -Wall -Wextra -Werror -Wpedantic -O2 -Wstrict-aliasing

test:
	${CXX} ${CXXFLAGS} test.cpp -o test

clean:
	rm -f test

.PHONY:
	clean
