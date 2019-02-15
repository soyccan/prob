// by:
// soytw
// sol-essen.cpp

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
    bool essen, discard;

    Term(string term, pair<int, int> son):
        term(term), son(son), essen(false), discard(false) {}

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

const int MAXN = 20000020;
vector<Term> F; // stores product terms (original and merged), at most (2 * MAXN) terms
vector<int> mark;
vector<int> require[MAXN];
int N, L, Q;
bool used[MAXN];

// #ifdef DBG
// ostream& fout = cout;
// #else
// stringstream fout;
// #endif

// template<typename T>
// ostream& operator<<(ostream& sout, vector<T> arr) {
//     sout << '[';
//     if (arr.size() >= 1) {
//         sout << arr[0] << "_e" << arr[0].essen << "_d" << arr[0].discard;
//         FOR(i, 1, arr.size())
//             sout << ", " << arr[i] << "_e" << arr[i].essen << "_d" << arr[i].discard;
//     }
//     sout << ']';
//     return sout;
// }

// template<typename T>
// ostream& operator<<(ostream& sout, pair<T,T> p) {
//     return sout << '(' << p.first << ',' << p.second << ')';
// }

// ostream& operator<<(ostream& sout, Term t) {
//     return sout << t.term;
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
                F[_u].discard = F[_v].discard = true;

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
        // fout << "round=" << round << " offset=" << offset << " nxt_offset="<< nxt_offset;
        // FOR(i, 0, F.size())
        //     fout << "round" << round << " " << F[i] << '\n';
        // fout << "Fsz=" << F.size() << endl;

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

// void output_prime_implicants() {
//     // fout << "mark:" <<mark<<endl;
//     // fout << "F:" <<F<<endl;
//     int i = 0, j = 0, cnt = 0;
//     while (i < F.size() && j < mark.size()) {
//         // fout << F[i] << ' ';
//         // fout << i << ' ' << j << ' ' << F.size() << endl;
//         cnt++;
//         if (i == mark[j] - 1) {
//             j++;
//             // fout << F[i] << endl;
//             cout << cnt << ' ';
//             cnt = 0;
//         }
//         i++;
//     }
//     // fout << i << ' ' << j << ' ' << F.size() << endl;
//     while (i < F.size())
//         cnt++, i++;
//     cout << cnt << '\n';
// }

int reduce_prime_implicants() {
    // fout << "F: " << F << endl;

    sort(F.begin() + N, F.end(), [](Term& a, Term& b) {
        if (a.discard != b.discard)
            return b.discard;
        if (a.essen != b.essen)
            return a.essen;
        return a.cover.size() > b.cover.size();
    });
    // fout << "F: " << F << endl;

    int i = N;
    while (i < F.size() && !F[i].discard) i++;
    F.erase(F.begin() + i, F.end());
    // fout << "F: " << F << endl;

    int essen_cnt = 0, disc_cnt = 0;
    FOR(i, 0, F.size())
        if (F[i].essen) essen_cnt++;
        else if (F[i].discard) disc_cnt++;

    int l = max(essen_cnt, 1), r = min(N, (int)F.size() - disc_cnt);
    while (l < r) {
        int k = (l + r) / 2;
        // fout << "l=" << l << " r=" << r << " k=" << k << endl;

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
    // assert(l == r);
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
    ios::sync_with_stdio(0);
    cin.tie(0);

    cin >> N >> L >> Q;
    FOR(i, 0, N) {
        string x;
        cin >> x;
        // assert(x.size() == L);
        F.push_back(Term(x, make_pair(i, i)));
    }

    // to be removed
    // input will guaranteed to be unique
    // sort(F.begin(), F.end());
    // F.erase(unique(F.begin(), F.end()), F.end());
    // assert(N == F.size());
    // N = F.size();
    // FOR(i, 0, N) {
    //     F[i].son = make_pair(i, i);
    // }

    find_prime_implicants();

    if (mark.size()) {
        cout << mark[0] << ' ';
        FOR(i, 1, mark.size())
            cout << mark[i] - mark[i-1] << ' ';
        cout << F.size() - mark[ mark.size() - 1 ] << '\n';
    }
    else
        cout << F.size() << '\n';

    // output_prime_implicants();

    // fout << "F: " << F << endl;

    if (Q == 2) {
        FOR(i, 0, F.size())
            if (!F[i].discard)
                build_cover(i, i);

        FOR(i, 0, F.size())
            if (!F[i].discard)
                for (int j : F[i].cover) {
                    // assert(j >= 0 && j <= N);
                    require[j].push_back(i);
                }
        FOR(i, 0, N) {
            // assert(require[i].size() > 0);
            if (require[i].size() == 1)
                F[ require[i][0] ].essen = true;
        }

        // FOR(i, 0, F.size()) {
        //     fout << "cover[" << F[i].term << "]: ";
        //     for (int j : F[i].cover)
        //         fout << F[j].term << ',';
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
