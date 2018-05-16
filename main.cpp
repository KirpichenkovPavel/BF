//
// Created by pavel on 04.04.18.
//
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "BF.h"
#include <cstring>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 5){
        cerr << "Pass graph file name, node index, number of threads, file name for profiling\n";
        return 1;
    }
    Graph g = parseGraph(argv[1]);
    bool doPrint = argc > 5 && !strcmp(argv[5], "-p");
    if (!validate(&g))
        return 2;
    if (doPrint)
        printGraph(g);

    findPath(&g, atoi(argv[2]), !doPrint, atoi(argv[3]), argv[4]);
    return 0;
}

