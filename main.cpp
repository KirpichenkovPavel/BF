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
    if (argc < 3){
        cerr << "Pass graph file name and node index\n";
        return 1;
    }
    Graph g = parseGraph(argv[1]);
    bool doPrint = argc == 4 && !strcmp(argv[3], "-p");
    if (!validate(&g))
        return 2;
    if (doPrint)
        printGraph(g);
    findPath(&g, atoi(argv[2]), !doPrint);
    return 0;
}

