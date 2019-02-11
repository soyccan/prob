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
#include <sstream>
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
vector<int> mark;
int N, L, Q;
bool used[MAXN];

#ifdef DBG
ostream& fout = cout;
#else
stringstream fout;
#endif

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

void bfs(int root, int loff, int roff) {
    queue<pair<int,int>> q; // (nxt, pa)
    q.push({root, -1});
    used[root] = true;
    while (!q.empty()) {
        int _u = q.front().first;
        int pa = q.front().second;
        q.pop();

        string u = F[_u].term;
        // fout << 'u' << u << "\n";

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
            // fout << "v" << v << " _v" << _v << endl;
            if (_v != roff && _v != pa && F[_v].term == v) {
                // fout << "u" << u << " -> v" << v << " pa=" << pa;
                v[i] = '-';
                // fout << " pushed_merged_term=" << v << endl;
                F.push_back(Term(v, make_pair(_u, _v)));

                if (!used[_v]) {
                    used[_v] = true;
                    q.push({_v, _u});
                }
            }
        }
        // fout << endl;
    }
}

void find_prime_implicants() {
    int offset = 0, nxt_offset = F.size();
    for (int round=0; round < 20 && 1 << round <= N; round++) {
        // fout << "round=" << round << " offset=" << offset << " nxt_offset="<< nxt_offset << " F:" << F << endl;

        memset(used, 0, sizeof used);
        FOR(i, offset, nxt_offset)
            if (!used[i])
                bfs(i, offset, nxt_offset);

        if (F.size() == nxt_offset) break;
        mark.push_back(nxt_offset);
        offset = nxt_offset;

        // TODO: find way to make it unique during BFS
        sort(F.begin() + offset, F.end());
        F.erase(unique(F.begin() + offset, F.end()), F.end());

        nxt_offset = F.size();
    }
}

void output_prime_implicants() {
    // fout << "mark:" <<mark<<endl;
    int i = 0, j = 0, cnt = 0;
    while (i < F.size() && j < mark.size()) {
        // fout << F[i] << ' ';
        cnt++;
        if (i == mark[j] - 1) {
            j++;
            // fout << F[i] << '\n';
            cout << cnt << ' ';
            cnt = 0;
        }
        i++;
    }
    while (i < F.size())
        cnt++, i++;
    cout << cnt << '\n';
}

int reduce_prime_implicants() {
    // fout << "F: " << F << endl;
    int l = 1, r = N;
    while (l < r) {
        int k = (l + r) / 2;
        // fout << "k=" << k << endl;

        bool suc = true;
        vector<bool> fil(F.size()); // filter
        FOR(i, F.size()-k, F.size()) fil[i] = true;
        do {
            suc = true;
            // fout << "fil:" << fil << endl;
            vector<bool> sel(N);
            FOR(i, 0, F.size()) {
                if (fil[i]) {
                    for (int j : F[i].cover) {
                        // fout << "cover[" << F[i] << "]=" << F[j] << endl;
                        sel[j] = true;
                    }
                }
            }
            FOR(i, 0, N)
                 if (!sel[i])
                    suc = false;
            // fout << "sel:" << sel << endl << endl;
            if (suc) break;
        } while (next_permutation(fil.begin(), fil.end()));

        if (suc) r = k;
        else l = k + 1;
    }
    assert(l == r);
    return r;
}

void build_cover(int rt, int i) {
    // fout << "rt=" << F[rt] << " F[" << i << "]=" << F[i] << " son=(" << F[ F[i].son.first ] << "," << F[ F[i].son.second ] << ")\n";

    if (F[i].son.first == i && F[i].son.second == i) {
        F[rt].cover.push_back(i);
        return;
    }

    build_cover(rt, F[i].son.first);
    build_cover(rt, F[i].son.second);
}

int main() {
    cin >> N >> L >> Q;
    FOR(i, 0, N) {
        string x;
        cin >> x;
        assert(x.size() == L);
        F.push_back(Term(x, make_pair(i, i)));
    }

    // to be removed
    // input will guaranteed to be unique
    sort(F.begin(), F.end());
    F.erase(unique(F.begin(), F.end()), F.end());
    N = F.size();
    FOR(i, 0, N) {
        F[i].son = make_pair(i, i);
    }

    find_prime_implicants();
    output_prime_implicants();

    if (Q == 2) {
        FOR(i, 0, F.size())
            build_cover(i, i);

        // FOR(i, 0, F.size()) {
        //     fout << "cover[" << F[i] << "]: ";
        //     for (int j : F[i].cover)
        //         fout << F[j] << ',';
        //     fout << endl;
        // }

        int r = reduce_prime_implicants();
        cout << r << '\n';
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
