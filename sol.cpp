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

#pragma GCC diagnostic ignored "-Wsign-compare"

#include <iostream>
#include <algorithm>
#include <exception>
#include <queue>
#include <utility>
#include <vector>
#include <set>
#include <cstring>
#include <cassert>
#define FOR(i,a,n) for (int i=a; i<n; i++)
using namespace std;


struct Term {
    string term;
    pair<int, int> son;
    vector<int> cover;

    Term(string term, pair<int, int> son): term(term), son(son) {}

    bool operator<(string s) const {
        return term < s;
    }
    bool operator<(Term t) const {
        return term < t.term;
    }
    bool operator==(Term t) const {
        return term == t.term;
    }
};

const int MAXN = 100020;
vector<Term> F; // stores product terms (original and merged), at most (2 * MAXN) terms
int N;
bool used[MAXN];

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

template<typename T>
ostream& operator<<(ostream& sout, pair<T,T> p) {
    return sout << '(' << p.first << ',' << p.second << ')';
}

ostream& operator<<(ostream& sout, Term t) {
    return sout << t.term;
}

// void init() {
//     F.clear();
//     memset(used, 0, sizeof used);
// }

// void dfs(int _u, int round, int pa) {
//     used[_u] = true;
//     const string& u = F[round][_u];
//     cout << 'u' << u << "\n";

//     // find all v such that u -> v
//     FOR(i,0,u.size()) {
//         string v = u;
//         if (v[i] == '0') v[i] = '1';
//         else if (v[i] == '1') v[i] = '0';
//         else if (v[i] == '-') continue;
//         else throw new exception();

//         // if v is in F
//         // TODO: cut prune: only when not visit is binary search necessary
//         // TODO: cut prune to reduce time complexity
//         int _v = lower_bound(F[round].begin(), F[round].end(), v) - F[round].begin();
//         // cout << "v" << v << " _v" << _v << endl;
//         if (_v != F[round].size() && _v != pa && F[round][_v] == v) {
//             cout << "u" << u << " -> v" << v;
//             v[i] = '-';
//             cout << " pushed: round=" << round << " merged_term=" << v << endl;
//             F[round+1].push_back(v);

//             if (!used[_v]) {
//                 dfs(_v, round, _u);
//             }
//         }
//     }
//     cout << endl;
// }

void bfs(int root, int loff, int roff) {
    queue<pair<int,int>> q; // (nxt, pa)
    q.push({root, -1});
    used[root] = true;
    while (!q.empty()) {
        int _u = q.front().first;
        int pa = q.front().second;
        q.pop();

        string u = F[_u].term;
        cout << 'u' << u << "\n";

        // find all v such that u -> v
        FOR(i, 0, u.size()) {
            string v = u;
            if (v[i] == '0') v[i] = '1';
            else if (v[i] == '1') v[i] = '0';
            else if (v[i] == '-') continue;
            else throw new exception();

            // if v is in F
            // TODO: cut prune: only when not visit is binary search necessary
            // TODO: cut prune to reduce time complexity
            int _v = lower_bound(F.begin() + loff, F.begin() + roff, v) - F.begin();
            // cout << "v" << v << " _v" << _v << endl;
            if (_v != roff && _v != pa && F[_v].term == v) {
                cout << "u" << u << " -> v" << v << " pa=" << pa;
                v[i] = '-';
                cout << " pushed_merged_term=" << v << endl;
                F.push_back(Term(v, make_pair(_u, _v)));

                if (!used[_v]) {
                    used[_v] = true;
                    q.push({_v, _u});
                }
            }
        }
        cout << endl;
    }
}

void find_prime_implicants() {
    int offset = 0, nxt_offset = F.size();
    for (int round=0; round < 20 && 1 << round <= N; round++) {
        cout << "round=" << round << " offset=" << offset << " nxt_offset="<< nxt_offset << " F:" << F << endl;

        memset(used, 0, sizeof used);
        FOR(i, offset, nxt_offset)
            if (!used[i])
                bfs(i, offset, nxt_offset);


        offset = nxt_offset;

        // TODO: find way to make it unique during BFS
        sort(F.begin() + offset, F.end());
        F.erase(unique(F.begin() + offset, F.end()), F.end());

        if (F.size() == nxt_offset) break;
        nxt_offset = F.size();
    }
}

int reduce_prime_implicants() {
    cout << "F: " << F << endl;
    int l = 1, r = N;
    while (l < r) {
        int k = (l + r) / 2;
        cout << "k=" << k << endl;

        bool suc = true;
        vector<bool> fil(F.size()); // filter
        FOR(i, F.size()-k, F.size()) fil[i] = true;
        do {
            suc = true;
            cout << "fil:" << fil << endl;
            vector<bool> sel(N);
            FOR(i, 0, F.size()) {
                if (fil[i]) {
                    for (int j : F[i].cover) {
                        cout << "cover[" << F[i] << "]=" << F[j] << endl;
                        sel[j] = true;
                    }
                }
            }
            FOR(i, 0, N)
                 if (!sel[i])
                    suc = false;
            cout << "sel:" << sel << endl << endl;
            if (suc) break;
        } while (next_permutation(fil.begin(), fil.end()));

        if (suc) r = k;
        else l = k + 1;
    }
    assert(l == r);
    return r;
}

void build_cover(int rt, int i) {
    cout << "rt=" << F[rt] << " F[" << i << "]=" << F[i] << " son=(" << F[ F[i].son.first ] << "," << F[ F[i].son.second ] << ")\n";

    if (F[i].son.first == i && F[i].son.second == i) {
        F[rt].cover.push_back(i);
        return;
    }

    build_cover(rt, F[i].son.first);
    build_cover(rt, F[i].son.second);
}

int main() {
    cin >> N;
    FOR(i, 0, N) {
        string x;
        cin >> x;
        F.push_back(Term(x, make_pair(i, i)));
    }

    find_prime_implicants();

    FOR(i, 0, F.size())
        build_cover(i, i);

    FOR(i, 0, F.size()) {
        cout << "cover[" << F[i] << "]: ";
        for (int j : F[i].cover)
            cout << F[j] << ',';
        cout << endl;
    }

    int r = reduce_prime_implicants();
    cout << r << endl;

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
