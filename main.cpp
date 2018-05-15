//
// Created by pavel on 04.04.18.
//
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "BF.h"
#include <cstring>
#include <mpi.h>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    if (argc < 3){
        cerr << "Pass graph file name and node index\n";
        return 1;
    }
    int currentId, numOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentId);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

        Graph g = parseGraph(argv[1]);
        bool doPrint = argc == 4 && !strcmp(argv[3], "-p");
    if (currentId == 0) {
        if (!validate(&g))
            return 2;
        if (doPrint);
        printGraph(g);
    }
    findPath(&g, atoi(argv[2]), !doPrint);
    MPI_Finalize();
    return 0;
}

