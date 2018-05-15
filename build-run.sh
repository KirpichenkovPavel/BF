#! /bin/bash
mpicxx BF.cpp main.cpp -o bellman-ford
if [ "$?" == 0 ]; then
while read p; do
    echo "running $p"
    sleep .1
    args=($p)
    mpiexec -np 4 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
done <test-config.txt
fi
