#! /bin/bash
mpicxx BF.cpp main.cpp -o bellman-ford -std=c++11
if [ "$?" == 0 ]; then
while read p; do
    echo "running $p"
    sleep .1
    args=($p)
    mpiexec -np 1 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
    mpiexec -np 2 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
    mpiexec -np 4 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
done <test-config.txt
fi
