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

const int MAXN = 20000020;
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

void output_prime_implicants() {
    // fout << "mark:" <<mark<<endl;
    // fout << "F:" <<F<<endl;
    int i = 0, j = 0, cnt = 0;
    while (i < F.size() && j < mark.size()) {
        // fout << F[i] << ' ';
        // fout << i << ' ' << j << ' ' << F.size() << endl;
        cnt++;
        if (i == mark[j] - 1) {
            j++;
            // fout << F[i] << endl;
            cout << cnt << ' ';
            cnt = 0;
        }
        i++;
    }
    // fout << i << ' ' << j << ' ' << F.size() << endl;
    while (i < F.size())
        cnt++, i++;
    cout << cnt << '\n';
}

unsigned ulog2(uint32_t v)
{ /* Evaluates [log2 v] */
  static const unsigned MUL_DE_BRUIJN_BIT[] =
  {
     0,  9,  1, 10, 13, 21,  2, 29, 11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7, 19, 27, 23,  6, 26,  5,  4, 31
  };

  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;

  return MUL_DE_BRUIJN_BIT[(v * 0x07C4ACDDu) >> 27];
}

int nxt(int x) {
    assert(x != 0);

    // int suf[32] = { 0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5 }; // hanoi tower
    // int psuf[32] = {0};
    // FOR(i, 1, 32) psuf[i] = psuf[i-1] + suf[i];

    int l = x + 1, r = (1U << 22) - 1U;
    while (l < r) { // TODO: verify the correctness of binary search
        int y = (l + r) / 2;
        if (y - x == psuf[y-1] - psuf[x-1])
            r = y;
        else
            l = y + 1;
    }
    assert(l == r);
    return l;
}

int reduce_prime_implicants() {
    fout << "F: " << F << endl;
    int l = 1, r = N;
    while (l < r) {
        int k = (l + r) / 2;
        fout << "k=" << k << endl;

        bool suc = true;
        int n = F.size();
        for (int fil = (1<<k) - 1; fil <= ( (1<<n) - (1<<(n-k)) ); fil=nxt(fil)) { // filter
            int aaaa = fil;
            fout <<"fil: ";
            while (aaaa > 0) {
                fout << (aaaa % 2);
                aaaa /= 2;
            }
            fout << endl;

            suc = true;
            vector<bool> sel(N);
            for (int i=0; i<n; i++) {
                if (fil >> i & 1) {
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
        }

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
    ios::sync_with_stdio(0);
    cin.tie(0);

    cin >> N >> L >> Q;
    FOR(i, 0, N) {
        string x;
        cin >> x;
        assert(x.size() == L);
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
