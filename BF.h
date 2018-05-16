//
// Created by pavel on 04.04.18.
//

#ifndef BF_BF_H
#define BF_BF_H

#include <string>

struct link {
    int targetId;
    int length;
    link();
};

struct node {
    int id;
    struct link * links;
    int linkNum;
    node();
};

struct graph {
    struct node * nodes;
    int size;
    graph();
};

typedef struct link Link;
typedef struct node Node;
typedef struct graph Graph;

Graph parseGraph(const char* file);
int printFile(const char *filename);
void printGraph(Graph newGraph);
bool updateEstimates(long *estimates, Graph *graph, int threadNumber);
void findPath(Graph *graph, int fromId, bool noprint, int threadNumber, const char *filename);
void printEstimates(long *estimates, int size);
bool validate(Graph *graph);

#endif //BF_BF_H
