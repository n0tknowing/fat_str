CXX=clang++-14
CXXFLAGS=-std=c++14 -Wall -Wextra -Werror -Wpedantic -O2 -g -fsanitize=address,undefined

test:
	${CXX} ${CXXFLAGS} test.cpp -o test

clean:
	rm -f test

.PHONY:
	clean
