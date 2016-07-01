Purpose:  Solve the nearest neigbour with a templatized K dimensional KDTree representing the spatial vector point. Use the KDTree to run nearest neigbour algorithm.r

This package consists of the following files and directory:

compileAndRun.sh
KDNode.hpp
kdTree.hpp
main.cpp
sample_data.csv
query_data.csv
bin directory


Dependency: The compileAndRun.sh expects the ksh binary to be present at /bin/ksh and g++ to be present at /usr/bin/g++, which are common to most Linux/unix architectures. 

Compiler details: The compilation was done with g++ version 4.2.1  on Apple LLVM version 7.3.0 (clang-703.0.31)

Target: x86_64-apple-darwin15.5.0


Details : The compileAndRun.sh script is expected to compile the binary "bin/nearestNeighbour" and run it. The main.cpp contains build_KDTree, query_KDTree implementation. The KDNode.hpp and kdTree.hpp have the templatized library implementation of KDTree and related interface/data-structures.


Usage : ./compileAndRun.sh <sample_data file path> <query_data file path> <filePath for saving the query results> <path for saving/loading KDTree>(optional)


Example : for sampleFile and queryFile in the same directory. Run below:

./compileAndRun.sh ./sample_data.csv ./query_data.csv ./saveResult.txt
