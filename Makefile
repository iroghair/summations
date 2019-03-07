all:
	g++ -o summation -O3 -fopenmp -std=c++14 main.cpp -lquadmath
