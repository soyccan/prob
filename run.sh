#!/bin/sh
GPP='g++ -Wall -Wextra -DEVAL -std=gnu++11 -O2 -pipe -static -s'
GPP_DBG='g++ -Wall -Wextra -std=gnu++11 -g -DDBG'

$GPP_DBG sol.cpp
if [ "$?" = "0" ]; then
    ./a.out < in.txt
fi
