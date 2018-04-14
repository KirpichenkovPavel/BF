#! /bin/bash
g++ BF.cpp main.cpp -o bellman-ford -lpthread
if [ "$?" == 0 ]; then
while read p; do
    echo "running $p"
    sleep .1
    args=($p)
    time taskset 0x1 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
    echo ""
    sleep .1
    time taskset 0x3 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
    echo ""
    sleep .1
    time taskset 0x7 ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
    echo ""
    sleep .1
    time taskset 0xF ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}"
    echo ""
done <test-config.txt
fi
