import os, sys
for i in os.scandir('.'):
    if i.path[-2:] != 'in':
        continue
    print(i.path, file=sys.stderr)
    sys.stdin = open(i.path)

    N, L, Q = map(int, input().split(' '))
    T = input().split(' ')
    T = list(set(int(t, 2) for t in T if t != ''))
    T.sort()
    assert N == len(T)
    N = len(T)

    # print('>', N, ',', L, ',', Q, file=sys.stderr)
    # print('>', T, len(T), file=sys.stderr)

    # if N != len(T) or not all(T) or ' ' in T or '' in T or len(set(T)) != len(T):
    #     print('error on ', i.path, file=sys.stderr)
    #     continue

    with open('out/' + i.path, 'w') as f:
        print(N, L, Q, file=f)
        print(' '.join(str(t) for t in T), file=f)
