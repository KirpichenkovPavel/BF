#! /usr/bin/python

def make_graph(num_of_nodes, file):
    with open(file, 'w') as file:
        file.write('{},\n'.format(num_of_nodes))
        for i in range(num_of_nodes):
            for j in range(num_of_nodes):
                if i == j:
                    file.write('0,')
                elif abs(i - j) < 3:
                    file.write('1,')
                else:
                    file.write(',')
            file.write('\n')

make_graph(10, 'tests/test2.txt')
make_graph(100, 'tests/test3.txt')
make_graph(1000, 'tests/test4.txt')
