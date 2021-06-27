
run : test.out
	./test.out

all : test.out test.out.abort
	
test.out.abort : test.cpp rangeset.hpp
	g++ -std=c++17 -DCXXTEST_HAVE_EH -DCXXTEST_ABORT_TEST_ON_FAIL -D_CXXTEST_HARD_ABORT test.cpp -O0 -g -o $@

test.out : test.cpp rangeset.hpp
	g++ -std=c++17 test.cpp -O0 -g -o $@

test.cpp : test.hpp
	cxxtestgen --runner=ParenPrinter --have-eh -o $@ $<

