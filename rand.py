import random

N = 6
print(N)
for _ in range(N):
    # s = random.randint(2**3, 2**4 - 1)
    s = random.choice(range(16))
    s = '{:04b}'.format(s)
    print(s, end=' ')


# output:
# line 1: N
# line 2 (N binary strings): 000011 100001 100101 ...
