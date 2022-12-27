CXX=clang++-14
CXXFLAGS=-std=c++11 -Wall -Wextra -Werror -Wpedantic -O2

test:
	${CXX} ${CXXFLAGS} test.cpp -o test

clean:
	rm -f test

.PHONY:
	clean
