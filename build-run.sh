#! /bin/bash
g++ BF.cpp main.cpp -o bellman-ford -lpthread -std=c++11
if [ "$?" == 0 ]; then
rm ./results/*
while read p; do
    echo "running $p 100 times"
    args=($p)
    for i in {1..100}; do
    ./bellman-ford "tests/${args[0]}" "${args[1]}" "${args[2]}" "${args[3]}" "${args[4]}"
    done
    echo ""
    sleep .1
done <test-config.txt
fi
