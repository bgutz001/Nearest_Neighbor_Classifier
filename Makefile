NearestNeighbor.out: main.cpp
	g++ -o NearestNeighbor.out -std=c++11 main.cpp

clean:
	rm NearestNeighbor.out
	rm *~
