all: main

main: queue.o generating.o servers.o optimization.o io.o spec.o statistics.o system.o
	g++ -std=c++11 ./main.cpp *.o

queue.o:
	g++ -std=c++11 -c queue.cpp
generating.o:
	g++ -std=c++11 -c generating.cpp
servers.o:
	g++ -std=c++11 -c servers.cpp
optimization.o:
	g++ -std=c++11 -c optimization.cpp
io.o:
	g++ -std=c++11 -c io.cpp
spec.o:
	g++ -std=c++11 -c spec.cpp
statistics.o:
	g++ -std=c++11 -c statistics.cpp
system.o:
	g++ -std=c++11 -c system.cpp
