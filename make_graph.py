#! /usr/bin/python
from itertools import combinations
from random import choice

def make_graph(num_of_nodes, file):
    lengths = {}
    for i,j in combinations(range(num_of_nodes), 2):
        length = choice(list(range(1, 1000)))
        lengths["{}-{}".format(i, j)] = length
        lengths["{}-{}".format(j, i)] = length
    for i in range(num_of_nodes):
        lengths["{}-{}".format(i,i)] = 0


    with open(file, 'w') as file:
        file.write('{},\n'.format(num_of_nodes))
        for i in range(num_of_nodes):
            for j in range(num_of_nodes):
                file.write("{},".format(lengths.get("{}-{}".format(i, j))))
            file.write('\n')

make_graph(10, 'tests/test2.txt')
make_graph(100, 'tests/test3.txt')
make_graph(1000, 'tests/test4.txt')
make_graph(2000, 'tests/test5.txt')
make_graph(3000, 'tests/test6.txt')