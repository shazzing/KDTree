all: nearest
nearest : main.o
CXX=/usr/bin/g++
CPPFLAGS = -std=c++11
main.o : main.cpp
	$(CXX) ${CPPFLAGS} -c -o bin/$(nearestNeighbour) main.cpp
nearest : $(nearestNeighbour)
	$(CXX) $(CPPFLAGS) bin/$(main.o) -o bin/nearest
clean : 
	rm bin/nearest $(nearestNeigbour)
