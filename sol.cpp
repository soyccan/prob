// by:
// soytw
//
// problem:
// given a boolean function by sum of minterms,
// simplify it as a sum of least product terms
//
// solution:
// depth-first search of implicit graph (or BFS?)
// find out all essential prime implicants
//
// range:
// L := number of arguments
// N := number of minterms
// N <= 2^L
// (undone...)
//
// time complexity:
// sort: L * N * Log(N)
// bfs: E + V
//     = N * L * L * log(N) + N
//     = L^2 * N * log(N)
//     <= L^2 * 2^L * log(2^L)
//     = L^3 * 2^L


#include <iostream>
#include <algorithm>
#include <exception>
#include <stack>
#include <vector>
#define FOR(i,a,n) for (int i=a; i<n; i++)
using namespace std;

const int MAXN = 100020;
vector<string> F[20]; // stores product terms, 1, 2, 4, 8, ...
int N;
bool used[20][MAXN];

template<typename T>
ostream& operator<<(ostream& sout, vector<T> arr) {
    sout << '[';
    if (arr.size() >= 1) {
        sout << arr[0];
        FOR(i, 1, arr.size())
            sout << ", " << arr[i];
    }
    sout << ']';
    return sout;
}

void dfs(int _u, int round, int pa) {
    used[round][_u] = true;
    const string& u = F[round][_u];
    cout << 'u' << u << "\n";

    // find all v such that u -> v
    FOR(i,0,u.size()) {
        string v = u;
        if (v[i] == '0') v[i] = '1';
        else if (v[i] == '1') v[i] = '0';
        else if (v[i] == '-') continue;
        else throw new exception();

        // if v is in F
        // TODO: cut prune: only when not visit is binary search necessary
        // TODO: cut prune to reduce time complexity
        int _v = lower_bound(F[round].begin(), F[round].end(), v) - F[round].begin();
        // cout << "v" << v << " _v" << _v << '\n';
        if (_v != F[round].size() && _v != pa && F[round][_v] == v) {
            cout << "u" << u << " -> v" << v;
            v[i] = '-';
            cout << " pushed: round=" << round << " merged_term=" << v << '\n';
            F[round+1].push_back(v);

            if (!used[round][_v]) {
                dfs(_v, round, _u);
            }
        }
    }
    cout << '\n';
}

// void bfs(int st) {
//     queue<int> q;
//     q.push(st);
//     while (!q.empty()) {
//         const string& u = F[q.front()]; q.pop();
//         cout << 'u' << u << "\n";

//         // find all v such that u -> v
//         FOR(i,0,u.size()) {
//             string v = u;
//             if (v[i] == '0') v[i] = '1';
//             else v[i] = '0';

//             // if v is in F
//             // TODO: cut prune: only when not visit is binary search necessary
//             int idx = lower_bound(F, F+N, v) - F;
//             if (idx != N && !used[idx]) {
//                 used[idx] = true;
//                 q.push(idx);

//                 cout << 'v' << v << '\n';
//             }
//         }
//         cout << '\n';
//     }
// }

int main() {
    cin >> N;
    FOR(i,0,N) {
        string x;
        cin >> x;
        F[0].push_back(x);
    }

    FOR(round,0,3) {
        sort(F[round].begin(), F[round].end());
        F[round].erase(unique(F[round].begin(), F[round].end()), F[round].end());
        cout << "F[" << round << "]: " << F[round] << '\n';

        FOR(i,0,F[round].size())
            if (!used[round][i])
                dfs(i, round, -1);
    }

    return 0;
}

/*
input:
line 1: N
line 2 (N binary strings): 000011 100001 100101 ...

test input:
6
0000 0001 0010 0100 0101 1000


xy\zw | 00 | 01 | 11 | 10
------+----+----+----+----
00    | 1  | 1  |    | 1
01    | 1  | 1  |    |
11    |    |    |    |
10    | 1  |    |    |


*/
