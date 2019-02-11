#!/usr/bin/env python3

from random import randint as R
from random import choice

N = 23
L = 8
Q = 2
T = set()

for _ in range(N):
    T.add(R(0, (1 << L) - 1))

N = len(T)
print(N, L, Q)

for s in T:
    s = '{:b}'.format(s)
    print('0' * (L - len(s)) + s, end=' ')


# output:
# line 1: N L Q
# line 2 (N binary strings): 000011 100001 100101 ...
