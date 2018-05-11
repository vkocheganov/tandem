CFLAGS=-O0

all: main


main: queue.o generating.o servers.o optimization.o io.o spec.o statistics.o system.o main.cpp
	g++ $(CFLAGS)  -std=c++11 ./main.cpp *.o

queue.o: queue.cpp
	g++ $(CFLAGS) -std=c++11 -c queue.cpp
generating.o: queue.cpp
	g++ $(CFLAGS) -std=c++11 -c generating.cpp
servers.o: servers.cpp
	g++ $(CFLAGS) -std=c++11 -c servers.cpp
optimization.o: optimization.cpp
	g++ $(CFLAGS) -std=c++11 -c optimization.cpp
io.o: io.cpp
	g++ $(CFLAGS) -std=c++11 -c io.cpp
spec.o: spec.cpp
	g++ $(CFLAGS) -std=c++11 -c spec.cpp
statistics.o: statistics.cpp
	g++ $(CFLAGS) -std=c++11 -c statistics.cpp
system.o: system.cpp
	g++ $(CFLAGS) -std=c++11 -c system.cpp

clean:
	rm -rf ./*.o ./a.out
