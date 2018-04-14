//
// Created by pavel on 04.04.18.
//

#include "BF.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <climits>
#include <pthread.h>
#include <assert.h>

#define NUM_THREADS 4
#define BUFFER_SIZE 10

using namespace std;

link::link() {
    this->targetId = 0;
    this->length = 0;
}

node::node() {
    this->id = 0;
    this->links = 0;
    this->linkNum = 0;
}

graph::graph() {
    this->size = 0;
    this->nodes = 0;
}

struct thread_args {
    int *nodeIds;
    int numOfIds;
    Graph *graph;
    long *estimates;
    bool *changesFlag;
};

int printFile(const char *filename) {
    ifstream ifs;

    ifs.open (filename, ifstream::in);

    char *line = new char[BUFFER_SIZE+1];

    while (true) {
        ifs.getline(line, BUFFER_SIZE+1, ',');
        int next = atoi(line);
        if (next == 0)
            break;
        printf("%d\n", atoi(line));
        if (ifs.peek() == '\n'){
            ifs.get();
        }
        if (ifs.peek() == EOF){
            break;
        }
    }
    ifs.close();
    delete[] line;
    return 0;
}

Graph parseGraph(const char* file) {

    Graph newGraph;
    ifstream ifs;
    char *linkLength = new char[BUFFER_SIZE+1];

    ifs.open (file, ifstream::in);
    ifs.getline(linkLength, BUFFER_SIZE+1);
    int size = atoi(linkLength);
    Node *nodes = new Node[size];

    for (int i = 0; i < size; i++){
        Link *links = new Link[size];
        for (int j = 0; j < size; j++) {
            Link newLink;
            newLink.targetId = j;
            ifs.getline(linkLength, BUFFER_SIZE+1, ',');
            int next = atoi(linkLength);
            if (next != 0)
                newLink.length = next;
            else
                newLink.length = INT_MAX;
            links[j] = newLink;
        }

        Node newNode;
        newNode.id = i;
        newNode.linkNum = size;
        newNode.links = links;
        nodes[i] = newNode;

        if (ifs.peek() == '\n'){
            ifs.get();
        }
        else {
            printf("%d", ifs.peek());
            printf("Wrong number of elements\n");
            exit(1);
        }
    }

    newGraph.nodes = nodes;
    newGraph.size = size;

    ifs.close();
    delete[] linkLength;

    return newGraph;
}

void printGraph(Graph graph) {
    for (int i = 0; i< graph.size; i++){
        printf("%d: ", graph.nodes[i].id);
        for (int j = 0; j < graph.nodes[i].linkNum; j++){
            if (graph.nodes[i].links[j].length != INT_MAX)
                printf("%3d ", graph.nodes[i].links[j].length);
            else
                printf("--- ");
        }
        printf("\n");
    }
}

void findPath(Graph *graph, int fromId, bool noprint) {
    long *estimates = new long[graph->size];
    bool somethingChanged = true;

    for (int i = 0; i < graph->size; i++) {
        estimates[i] = i == fromId ? 0 : LONG_MAX;
    }
    while (somethingChanged) {
        somethingChanged = updateEstimates(estimates, graph);
    };
    if (!noprint)
        printEstimates(estimates, graph->size);

    delete[] estimates;
}

void* updateNodeEstimate(void *thread_args) {
    struct thread_args *args = (struct thread_args*) thread_args;
    Graph *graph = args->graph;
    long *estimates = args->estimates;
    int *nodeIds = args->nodeIds;
    int numOfIds = args->numOfIds;
    for (int i = 0; i < numOfIds; i++) {
        int nodeId = nodeIds[i];
        for (int j = 0; j < graph->nodes[nodeId].linkNum; j++) {
            Link *link = &(graph->nodes[nodeId].links[j]);

            long *prev = &(estimates[nodeId]);
            long curr = long(estimates[link->targetId] < LONG_MAX ?
                             estimates[link->targetId] + link->length : LONG_MAX);
            if (curr < *prev) {
                *prev = curr;
                *(args->changesFlag) = true;
            }
        }
    }
}

bool updateEstimates(long *estimates, Graph *graph) {
    bool somethingChanged = false;
    int numOfThreads = NUM_THREADS;

    pthread_t threads[numOfThreads];
    struct thread_args args[numOfThreads];
    int resultCode;
    int **ids = new int*[numOfThreads];
    int *numOfIds = new int[numOfThreads];

    // prepare indexes for threads
    for (int j = 0; j < numOfThreads; j++) {
        ids[j] = new int[graph->size / numOfThreads + 1];
        numOfIds[j] = 0;
    }
    for (int i = 0; i < graph->size; i++) {
        ids[i % numOfThreads][i / numOfThreads] = i;
        numOfIds[i % numOfThreads]++;
    }

    // make iteration
    for (int i = 0; i < numOfThreads; i++) {
        args[i].changesFlag = &somethingChanged;
        args[i].estimates = estimates;
        args[i].graph = graph;
        args[i].nodeIds = ids[i];
        args[i].numOfIds = numOfIds[i];
        resultCode = pthread_create(&threads[i], NULL, updateNodeEstimate, &args[i]);
        assert(!resultCode);
    }
    for (int i = 0; i < numOfThreads; i++) {
        resultCode = pthread_join(threads[i], NULL);
        assert(!resultCode);
    }

    // cleanup
    for (int j = 0; j < numOfThreads; j++) {
        delete[] ids[j];
    }
    delete[] ids;
    delete[] numOfIds;

    return somethingChanged;
}

void printEstimates(long *estimates, int size) {
    long max = 0;
    int len = 0;
    char *format = new char[6];

    for (int i = 0; i < size; i++) {
        if (estimates[i] > max)
            max = estimates[i];
    }
    while (max > 0){
        max = max / 10;
        len++;
    }
    sprintf(format, "%%%dld", len+1);
    for (int i = 0; i < size; i++) {
        printf(format, estimates[i]);
    }
}

bool validate(Graph *graph) {
    for (int i = 0; i < graph->size; i++) {
        for (int j = 0; j < graph->size; j++)
            if (graph->nodes[i].links[j].length != graph->nodes[j].links[i].length) {
                cerr << "Nodes " << i << " and " << j << " have different link length with each other";
                return false;
            }
    }
    return true;
}