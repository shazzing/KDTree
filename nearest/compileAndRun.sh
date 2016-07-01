#!/bin/ksh
rm -f bin/*.o bin/nearestNeighbour
g++ -std=c++11 -c -o bin/main.o main.cpp
g++ -std=c++11 bin/main.o -o bin/nearestNeighbour
chmod 755 ./bin/nearestNeighbour
if [ $# -lt 3  ]
then
	echo "Usage: $0 <sample_data file path> <query_data file path> <fileName for saving the query results> <path for saving/loading KDTree>(optional)"
	exit 1
else
#usage nearestNeigbour <sample_data file path> <query_data file path> <fileName for saving the query results> <path for saving/loading KDTree>(optional)
	./bin/nearestNeighbour ./sample_data.csv ./query_data.csv ./saveResult.txt
	exit 0
fi
