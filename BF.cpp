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
#include <mpi.h>
#include <unistd.h>

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
    int currId;
    MPI_Comm_rank(MPI_COMM_WORLD, &currId);

    long *estimates = new long[graph->size];
    bool somethingChanged = true;

    for (int i = 0; i < graph->size; i++) {
        estimates[i] = i == fromId ? 0 : LONG_MAX;
    }
    while (somethingChanged) {
        MPI_Bcast(estimates, graph->size, MPI_LONG, 0, MPI_COMM_WORLD);
        somethingChanged = updateEstimates(estimates, graph);
        MPI_Bcast(&somethingChanged, 1, MPI_BYTE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    };
    if (!noprint && currId == 0)
        printEstimates(estimates, graph->size);

    delete[] estimates;
}

void updateNodeEstimate(Graph *graph, long* estimates, int* nodeIds, int numOfIds, bool *changesFlag) {
    for (int i = 0; i < numOfIds; i++) {
        int nodeId = nodeIds[i];
        for (int j = 0; j < graph->nodes[nodeId].linkNum; j++) {
            Link *link = &(graph->nodes[nodeId].links[j]);

            long *prev = estimates + nodeId;
            long curr = estimates[link->targetId] < LONG_MAX && link->length < INT_MAX ?
                             estimates[link->targetId] + (long)(link->length) : LONG_MAX;
            if (curr < *prev) {
                *prev = curr;
                *changesFlag = true;
            }
        }
    }
}

bool updateEstimates(long *estimates, Graph *graph) {
    int currentId, numOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentId);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    int numOfThreads = numOfProcesses;
    int **ids;
    int *numOfIds;
    bool somethingChanged = false;
    MPI_Status status;

    ids = new int *[numOfThreads];
    numOfIds = new int[numOfThreads];

    // prepare indexes
    for (int j = 0; j < numOfThreads; j++) {
        ids[j] = new int[graph->size / numOfThreads + 1];
        numOfIds[j] = 0;
    }
    for (int i = 0; i < graph->size; i++) {
        ids[i % numOfThreads][i / numOfThreads] = i;
        numOfIds[i % numOfThreads]++;
    }

    updateNodeEstimate(graph, estimates, ids[currentId], numOfIds[currentId], &somethingChanged);
    if (currentId == 0) {
        for (int i = 0; i < graph->size; i++) {
            int sender = i % numOfThreads;
            if (sender) {
                MPI_Recv(estimates + i, 1, MPI_LONG, sender, 0, MPI_COMM_WORLD, &status);
            }
        }
    }
    else {
        for (int i = 0; i < numOfIds[currentId]; i++) {
            MPI_Send(estimates + ids[currentId][i], 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
        }
    }
    bool changeReceiver = somethingChanged;
    if (currentId == 0) {
        for (int i = 1; i < numOfProcesses; i++){
            MPI_Recv(&changeReceiver, 1, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);
            somethingChanged = somethingChanged ? somethingChanged : changeReceiver;
        }
    }
    else {
        MPI_Send(&somethingChanged, 1, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
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