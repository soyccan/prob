#!/bin/sh
GPP='g++ -Wall -Wextra -DEVAL -std=gnu++11 -O2 -pipe -static -s'
GPP_DBG='g++ -Wall -Wextra -std=gnu++11 -g'

# python3 rand.py > in.txt
# cat in.txt

$GPP_DBG sol.cpp
if [ "$?" = "0" ]; then
    for f in testcases/*.in; do
        ./a.out < $f | diff - "${f%.in}.out"
        if [ "$?" != "0" ]; then
            echo "Answer failed on $f"
            cat $f
        fi
    done
fi
