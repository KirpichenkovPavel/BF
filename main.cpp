//
// Created by pavel on 04.04.18.
//
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "BF.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3){
        cerr << "Pass graph file name and node index\n";
        return 1;
    }
    Graph g = parseGraph(argv[1]);
    if (!validate(&g))
        return 2;
    bool print = argc == 4 && argv[3] == "-p";
    if (print)
        printGraph(g);
    findPath(&g, atoi(argv[2]), print);
    return 0;
}

