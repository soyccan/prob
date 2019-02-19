// soytw

#include <cstdio>
#include <cassert>
#include <algorithm>
#define FOR(i, a, N) for (int i=a; i<N; i++)
using namespace std;

int A[40][40];
int N;

bool gauss() {
    FOR(i, 0, N) {
        int p = i;
        while (A[p][i] == 0 && p < N) p++;
        if (p == N) return false;
        if (p != i)
            FOR(k, 0, N)
                swap(A[p][k], A[i][k]);
        FOR(j, i+1, N) {
            assert(A[i][i] != 0);
            int c = A[j][i] / A[i][i];
            FOR(k, 0, N) {
                A[j][k] -= c * A[i][k];
                swap(A[j][k], A[i][k]);
            }
        }

        FOR(i, 0, N) {
            FOR(j, 0, N)
                printf("%+2d ", A[i][j]);
            putchar('\n');
        }
        putchar('\n');
    }
    return true;
}

int main() {
    // freopen("in.txt", "r", stdin);
    int t;
    scanf("%d", &t);
    while (t--) {
        scanf("%d", &N);
        FOR(i, 0, N)
            FOR(j, 0, N)
                scanf("%d", &A[i][j]);
        bool ans = true;
        if (gauss()) {
            FOR(i, 0, N)
                ans &= A[i][i];
        }
        else
            ans = false;
        puts(ans ? "YES" : "NO");
    }
    return 0;
}
