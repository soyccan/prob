#!/bin/sh
GPP='g++ -Wall -Wextra -Dsoyccan -DEVAL -std=gnu++11 -O2 -pipe -static -s'
GPP_DBG='g++ -Wall -Wextra -Dsoyccan -DEVAL -std=gnu++11 -g'

# python3 rand.py > in.txt
cat in.txt
$GPP_DBG sol.cpp
[ "$?" = "0" ] && ./a.out < in.txt
