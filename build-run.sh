#! /bin/bash
g++ BF.cpp main.cpp -o bellman-ford -lpthread
if [ "$?" == 0 ]; then
while read p; do
    echo "#######################################"
    args=($p)
    ./bellman-ford "tests/${args[0]}" "${args[1]}"
    echo ""
done <test-config.txt
fi
