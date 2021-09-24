
run : test.out
	./test.out
	
test.out : test.cpp rangeset.hpp
	g++ -std=c++17 test.cpp -O0 -g -o $@

