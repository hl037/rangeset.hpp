
run : test.out
	./test.out

coverage: run
	mkdir -p _me_coverage_cpp ; gcovr -r . --html-details -o ./_me_coverage_cpp/cov.html
	
test.out : test.cpp rangeset.hpp
	g++ -std=c++17 --coverage test.cpp -O0 -g -o $@

