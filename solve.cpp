#pragma region my_template
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;
using ll = long long;
using pi = pair<int, int>;
using ti = tuple<int, int, int>;
using vi = vector<int>;
using vvi = vector<vi>;

#define range(i, l, r) for(int i = (int)(l); i < (int)(r); i++)
#define rrange(i, l, r) for(int i = (int)(r)-1; i >= (int)(l); i--)
#define rep(i, n) range(i, 0, n)
#define rrep(i, n) rrange(i, 0, n)
#define len(a) ((int)(a).size())
#define all(a) (a).begin(), (a).end()
#define rall(a) (a).rbegin(), (a).rend()
#define uni(a) (a).erase(unique(all(a)), (a).end());

constexpr int INF = 1e9;
constexpr ll LINF = 1e18;

template<typename T>
istream &operator >> (istream &in, vector<T> &a){
    for(T &x: a) in >> x;
    return in;
}
template<typename T, typename U>
istream &operator >> (istream &in, pair<T, U> &a){
    in >> a.first >> a.second;
    return in;
}
template<typename T>
ostream &operator << (ostream &out, const vector<T> &a) {
    rep(i, len(a)) out << a[i] << (i == len(a)-1 ? "" : " ");
    return out;
}
template<typename T, typename U>
ostream &operator << (ostream &out, const pair<T, U> &a){
    out << a.first << " " << a.second;
    return out;
}
inline void print() { cout << "\n"; }
template <typename T, typename ...U>
inline void print(const T &t, const U &...u) {
    cout << t;
    if (sizeof...(u)) cout << " ";
    print(u...);
}
ofstream dout("./dump.txt");

inline void dump() { dout << "\n"; }
template <typename T, typename ...U>
inline void dump(const T &t, const U &...u) {
    dout << t;
    if (sizeof...(u)) dout << " ";
    dump(u...);
}

template<typename T> inline bool chmax(T &a, T b) { if (a < b) { a = b; return 1; } return 0; }
template<typename T> inline bool chmin(T &a, T b) { if (a > b) { a = b; return 1; } return 0; }

#pragma endregion

namespace util {
    struct Timer {
        static const uint64_t CYCLES_PER_SEC = 3e9;
        uint64_t start;
    
        Timer() : start{} { reset(); }
    
        void reset() { start = get_cycle(); }
    
        inline double get() const { return (double) get_cycle() / CYCLES_PER_SEC; }

    private:
        inline uint64_t get_cycle() const {
            unsigned low, high;
            __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
            return (((uint64_t) low) | ((uint64_t) high << 32ull)) - start;
        }
    };
    class XorShift {
        unsigned x, y, z, w; 
    public:    
        XorShift() {
            x = 123456789;
            y = 362436069;
            z = 521288629;
            w = 88675123;
        }
        inline unsigned next() {
            unsigned t = x ^ (x << 11);
            x = y; y = z; z = w;
            return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        }
        unsigned nextInt(int n) {
            return next() % n;
        }
        unsigned nextInt(int l, int r) {
            return l + (next() % (r - l));
        }
        double nextDouble() {
            return next() / 4294967295.0;
        }
        //参照を返すわけではないので注意
        template<typename T>
        T choice(const vector<T> &s) {
            assert (len(s) > 0);
            T val = s[nextInt(len(s))];
            return val;
        }

    };
    
    vector<pi> arr4 = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
    struct UnionFind {
    vi data;
    UnionFind(int n) : data(n, -1) {}
    
    int find(int x) {
        if (data[x] < 0) return x;
        return data[x] = find(data[x]);
    }
    
    void unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return;
        if (data[x] > data[y]) swap(x, y);
        data[x] += data[y];
        data[y] = x;
    }
    
    bool same(int x, int y) {
        return find(x) == find(y);
    }
    
    int size(int x) {
        return -data[find(x)];
    }
};

}
util::Timer timer;
util::XorShift rnd;

namespace inputs {
    int N, T;
    vector<string> t;
    void input() {
        cin >> N >> T;
        t = vector<string>(N); cin >> t;
    }
}
using inputs::N, inputs::T;
int idx2(int y, int x) { return y*N+x; }

/*---------------------------------------------*/
vector<string> symbols = {
    " ", "┥", "┸", "┛",
    "┝", "━", "┗", "┻",
    "┰", "┓", "┃", "┫",
    "┏", "┳", "┣", "╋" };

struct Tile {
    const int id, type;
    Tile(const int id, const int type) : id(id), type(type) {}
    int is_left() const {
        return type & 1;
    }
    int is_up() const {
        return (type >> 1) & 1;
    }
    int is_right() const {
        return (type >> 2) & 1;
    }
    int is_down() const {
        return (type >> 3) & 1;
    }
    friend ostream& operator << (ostream &out, const Tile &tile);
};
ostream& operator << (ostream &out, const Tile &tile) {
    out << symbols[tile.type];
    return out;
}
vector<Tile> tiles; //タイル一覧 初期位置によってIDが振られている
int edge_num = 0; //タイルの線の総数

void visualize_board(vvi &board) {
    rep(i, N) {
        rep(j, N) cout << tiles[board[i][j]];        
        cout << "\n";
    }
}



double eval_tree(vvi &board) {
    util::UnionFind uf(N*N);
    rep(y, N) rep(x, N) {
        if (y > 0 and tiles[board[y][x]].is_up() and tiles[board[y-1][x]].is_down()) uf.unite(idx2(y, x), idx2(y-1, x));
        if (x > 0 and tiles[board[y][x]].is_left() and tiles[board[y][x-1]].is_right()) uf.unite(idx2(y, x), idx2(y, x-1));
        if (y < N-1 and tiles[board[y][x]].is_down() and tiles[board[y+1][x]].is_up()) uf.unite(idx2(y, x), idx2(y+1, x));
        if (x < N-1 and tiles[board[y][x]].is_right() and tiles[board[y][x+1]].is_left()) uf.unite(idx2(y, x), idx2(y, x+1));
    }
    vvi count(N, vi(N));
    int max_size = 0;
    rep(i, N) rep(j, N) {
        count[i][j] = uf.size(idx2(i, j));
        chmax(max_size, count[i][j]);
    }
    int score = 0; //最大の木に属すタイルの有効な辺の合計
    int root = -1;
    rep(y, N) rep(x, N) {
        if (count[y][x] != max_size) continue;
        if (root == -1) root = uf.find(idx2(y, x));
        else if (root != uf.find(idx2(y, x))) continue;

        if (y > 0 and tiles[board[y][x]].is_up() and tiles[board[y-1][x]].is_down()) score++;
        if (x > 0 and tiles[board[y][x]].is_left() and tiles[board[y][x-1]].is_right())score++ ;
        if (y < N-1 and tiles[board[y][x]].is_down() and tiles[board[y+1][x]].is_up()) score++;
        if (x < N-1 and tiles[board[y][x]].is_right() and tiles[board[y][x+1]].is_left()) score++;
    }
    return (double) score / (double) edge_num;
}

void initialize(vvi &board) {
    unordered_set<int> used;

    //周りから埋める
    rep(y, N) rep(x, N) {
        if (y > 0 and y < N-1 and x > 0 and x < N-1) continue;
        vi cands;
        for (auto &tile: tiles) {
            if (used.count(tile.id) > 0) continue;
            if (y == 0 and tile.is_up()) continue;
            if (x == 0 and tile.is_left()) continue;
            if (y == N-1 and tile.is_down()) continue;
            if (x == N-1 and tile.is_right()) continue;
            cands.push_back(tile.id);
        }
        int t = rnd.choice(cands);
        board[y][x] = t;
        used.insert(t);
    }

    range(y, 1, N-1) range(x, 1, N-1) {
        vi cands;
        for (auto &tile: tiles) {
            if (used.count(tile.id) > 0) continue;
            cands.push_back(tile.id);
        }
        int t = rnd.choice(cands);
        board[y][x] = t;
        used.insert(t);
    }
}

void generate_spanning_tree() {
    vvi board(N, vi(N)); //IDによってタイルの位置を表す
    initialize(board);

    double score = eval_tree(board);
    print(score);
    visualize_board(board);

    int loopnum = 100000;
    int cnt = 0;
    while (cnt < loopnum) {
        int v1 = rnd.nextInt(N*N), v2 = rnd.nextInt(N*N);
        if (v1 == v2) continue;
        int y1 = v1/N, y2 = v2/N;
        int x1 = v1%N, x2 = v2%N;
        int &b1 = board[y1][x1], &b2 = board[y2][x2];
        if (tiles[b1].type == tiles[b2].type) continue;

        
        if (y1 == 0 and tiles[b2].is_up()) continue;
        if (x1 == 0 and tiles[b2].is_left()) continue;
        if (y1 == N-1 and tiles[b2].is_down()) continue;
        if (x1 == N-1 and tiles[b2].is_right()) continue;

        if (y2 == 0 and tiles[b1].is_up()) continue;
        if (x2 == 0 and tiles[b1].is_left()) continue;
        if (y2 == N-1 and tiles[b1].is_down()) continue;
        if (x2 == N-1 and tiles[b1].is_right()) continue;

        
        swap(b1, b2);
        double nscore = eval_tree(board);
        double prob = (1.0 - (double)cnt/(double)loopnum) * 0.4;
        if (score < nscore or rnd.nextDouble() < prob) score = nscore;
        else swap(b1, b2);
        cnt++;
    }
    score = eval_tree(board);
    print(score);
    visualize_board(board);
}

void init() {
    inputs::input();
    rep(i, N) rep(j, N) {
        int type = stoi(string(1, inputs::t[i][j]), nullptr, 16);
        tiles.push_back(Tile(idx2(i, j), type));
        Tile &t = tiles[idx2(i, j)];
        edge_num += t.is_left() + t.is_up() + t.is_right() + t.is_down();
    }
    print(edge_num);
    generate_spanning_tree();
}

int main() {
    cin.tie(0);
    ios::sync_with_stdio(false);
    init();
    return 0;
}
