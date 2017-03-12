all: NearestNeighbor.out ONearestNeighbor.out

NearestNeighbor.out: main.cpp
	g++ -o NearestNeighbor.out -std=c++11 main.cpp

ONearestNeighbor.out: main.cpp
	g++ -o ONearestNeighbor.out -std=c++11 -O3 main.cpp

clean:
	rm NearestNeighbor.out
	rm ONearestNeighbor.out
