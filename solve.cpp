#pragma region my_template
#include <algorithm>
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
            T val = s[nextInt(len(s))];
            return val;
        }

    };
    vector<pi> arr4 = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
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

void generate_spanning_tree() {
    vi used(len(tiles));
    vvi board(N, vi(N)); //IDによってタイルの位置を表す
    int y = 0, x = 0;
    while (true) {
        //現在のマスを配置可能なランダムなタイルで埋める
        vi cands; //候補
        for (auto &t: tiles) {
            if (used)
            if (x == 0 and t.is_left()) continue;
            if (y == 0 and t.is_up()) continue;
            if (x == N-1 and t.is_right()) continue;
            if (y == N-1 and t.is_down()) continue;
            cands.push_back(t.id);
        }
        Tile &tile = rnd.choice(cands);
        used[tile.id] = 1;

        vector<pi> nxts;
        for (auto [vy, vx]: util::arr4) {
            int ny = y+vy, nx = x+vx;
            if (ny < 0 or nx < 0 or ny >= N or nx >= N) continue;
            if (board[ny][nx] == 0) nxts.push_back({ny, nx});
        }
        if (nxts.empty()) break;
        auto [ny, nx] = rnd.choice(nxts);
        y = ny, x = nx;
    }
}

void init() {
    inputs::input();
    rep(i, N) rep(j, N) {
        int type = stoi(string(1, inputs::t[i][j]), nullptr, 16);
        tiles.push_back(Tile(i*N+j, type));
    }
    rep(i, N) {
        rep(j, N) cout << tiles[i*N+j];        
        cout << "\n";
    }
}

int main() {
    cin.tie(0);
    ios::sync_with_stdio(false);
    init();
    return 0;
}
