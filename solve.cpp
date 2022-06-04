// #define DEBUG_MODE

#pragma region my_template
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <stack>
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

#ifdef DEBUG_MODE
ofstream dout("./dump.txt");
#else
ofstream dout("/dev/null");
#endif

inline void dump() { dout << endl; }
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
            return next() %n;
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
int L;
int idx2(int y, int x) { return y*N+x; }
pi idx2(int i) {return {i/N, i%N}; }

/*---------------------------------------------*/
vector<string> symbols = {
    "▓", "┥", "┸", "┛",
    "┝", "━", "┗", "┻",
    "┰", "┓", "┃", "┫",
    "┏", "┳", "┣", "╋", "o" };

struct Tile {
    enum Type {
        BLANK, LEFT, UP, LU,
        RIGHT, LR, UR, LUR,
        DOWN, LD, UD, LUD,
        RD, LRD, URD, LURD, RES
    };

    const int id;
    const Type type;
    Tile(const int id, const Type type) : id(id), type(type) {}
    
    static int is_left(int t) {
        return t & 1;
    }
    static int is_up(int t) {
        return (t >> 1) & 1;
    }
    static int is_right(int t) {
        return (t >> 2) & 1;
    }
    static int is_down(int t) {
        return (t >> 3) & 1;
    }
    static int edge_num(int t) {
        return is_left(t) + is_up(t) + is_right(t) + is_down(t);
    }
    friend ostream& operator << (ostream &out, const Tile &tile);
};
ostream& operator << (ostream &out, const Tile &tile) {
    out << symbols[tile.type];
    return out;
}
vector<Tile> tiles; //タイル一覧 初期位置によってIDが振られている
int all_edge_num = 0; //タイルの線の総数

void visualize_board(vvi &board) {
    rep(i, N) {
        rep(j, N) cout << symbols[board[i][j]];        
        cout << "\n";
    }
    print("---------------");
}
void dump_board(vvi &board) {
    rep(i, N) {
        rep(j, N) dout << symbols[board[i][j]];        
        dout << endl;
    }
    dump("---------------");
}

namespace solve1 {

    struct State {
        vvi board; //タイルの種類によって盤面を表す
        vi remain_tile; //種類別の使用していないタイルの数
        queue<pi> q; //{ 現在のマス(一次元), 前回のマスの方向(LURD) }
        State() {
            board = vvi(N, vi(N));
            remain_tile = vi(16);
            for (Tile &t: tiles) {
                remain_tile[t.type]++;
            }
            q.push({0, -1});
        }
        void search() {
            while (!q.empty()) {
                auto [v, pd] = q.front(); q.pop();
                auto [y, x] = idx2(v);
                put(v, pd);
                if (pd != 0 and Tile::is_left(board[y][x])) {
                    q.push({idx2(y, x-1), 2});
                    board[y][x-1] = Tile::Type::RES;
                }
                if (pd != 1 and Tile::is_up(board[y][x])) {
                    q.push({idx2(y-1, x), 3});
                    board[y-1][x] = Tile::Type::RES;
                }
                if (pd != 2 and Tile::is_right(board[y][x])) {
                    q.push({idx2(y, x+1), 0});
                    board[y][x+1] = Tile::Type::RES;
                }
                if (pd != 3 and Tile::is_down(board[y][x])) {
                    q.push({idx2(y+1, x), 1});
                    board[y+1][x] = Tile::Type::RES;
                }

                dump_board(board);
                
            }
        }
        void put(int v, int pd) {
            auto [cy, cx] = idx2(v);

            vector<double> cand_score(16);

 
            range(t, 1, 16) {
                if (remain_tile[t] == 0) continue;

                //前のマスと繋がっているか
                if (pd == 0 and !Tile::is_left(t)) continue;
                if (pd == 1 and !Tile::is_up(t)) continue;
                if (pd == 2 and !Tile::is_right(t)) continue;
                if (pd == 3 and !Tile::is_down(t)) continue;

                double score = Tile::edge_num(t);
                const double ads = 0.5;

                //繋げる先の評価
                if (pd != 0 and Tile::is_left(t)) {
                    if (cx > 0 and board[cy][cx-1] == 0) {
                        for (auto [vy, vx]: util::arr4) {
                            int ny = cy+vy, nx = cx-1+vx;
                            if (nx < 0 or ny < 0 or nx >= N or ny >= N) score += ads;
                            else if (board[ny][nx] != 0 and board[ny][nx] != Tile::Type::RES) score += ads;
                        }
                    } else {
                        score = 0;
                        continue;
                    }
                }
                if (pd != 1 and Tile::is_up(t)) {
                    if (cy > 0 and board[cy-1][cx] == 0) {
                        for (auto [vy, vx]: util::arr4) {
                            int ny = cy-1+vy, nx = cx+vx;
                            if (nx < 0 or ny < 0 or nx >= N or ny >= N) score += ads;
                            else if (board[ny][nx] != 0 and board[ny][nx] != Tile::Type::RES) score += ads;
                        } 
                    } else {
                        score = 0;
                        continue;
                    }
                }
                if (pd != 2 and Tile::is_right(t)) {
                    if (cx < N-1 and board[cy][cx+1] == 0) {
                        for (auto [vy, vx]: util::arr4) {
                            int ny = cy+vy, nx = cx+1+vx;
                            if (nx < 0 or ny < 0 or nx >= N or ny >= N) score += ads;
                            else if (board[ny][nx] != 0 and board[ny][nx] != Tile::Type::RES) score += ads;
                        } 
                    } else {
                        score = 0;
                        continue;
                    }
                }
                if (pd != 3 and Tile::is_down(t)) {
                    if (cy < N-1 and board[cy+1][cx] == 0) {
                        for (auto [vy, vx]: util::arr4) {
                            int ny = cy+1+vy, nx = cx+vx;
                            if (nx < 0 or ny < 0 or nx >= N or ny >= N) score += ads;
                            else if (board[ny][nx] != 0 and board[ny][nx] != Tile::Type::RES) score += ads;
                        } 
                    } else {
                        score = 0;
                        continue;
                    }
                }
                cand_score[t] = score + rnd.nextDouble() * 0.01;
            }
            vector<pair<double, int>> cands;
            rep(i, 16) {
                if (cand_score[i] != 0) cands.push_back({cand_score[i], i});
            }
            if (cands.empty()) return;
            sort(rall(cands));
            
            rep(i, len(cands)) dump(cands[i]);

            int choose_t = cands[rnd.nextInt(min(len(cands), 2))].second;
            remain_tile[choose_t]--;
            board[cy][cx] = choose_t;
        }
    };
    
    double eval_tree(vvi &board) {
        util::UnionFind uf(L);
        rep(y, N) rep(x, N) {
            if (y > 0 and Tile::is_up(board[y][x]) and Tile::is_down(board[y-1][x])) uf.unite(idx2(y, x), idx2(y-1, x));
            if (x > 0 and Tile::is_left(board[y][x]) and Tile::is_right(board[y][x-1])) uf.unite(idx2(y, x), idx2(y, x-1));
            if (y < N-1 and Tile::is_down(board[y][x]) and Tile::is_up(board[y+1][x])) uf.unite(idx2(y, x), idx2(y+1, x));
            if (x < N-1 and Tile::is_right(board[y][x]) and Tile::is_left(board[y][x+1])) uf.unite(idx2(y, x), idx2(y, x+1));
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

            if (y > 0 and Tile::is_up(board[y][x]) and Tile::is_down(board[y-1][x])) score++;
            if (x > 0 and Tile::is_left(board[y][x]) and Tile::is_right(board[y][x-1])) score++;
            if (y < N-1 and Tile::is_down(board[y][x]) and Tile::is_up(board[y+1][x])) score++;
            if (x < N-1 and Tile::is_right(board[y][x]) and Tile::is_left(board[y][x+1])) score++;
        }
        return (double) score / (double) all_edge_num;
    }

    double generate_spanning_tree() {
        State s = State();
        s.search();
        double score = eval_tree(s.board);
        if (score == 1)
        visualize_board(s.board);

        return score;
    }
}

void init() {
    inputs::input();
    L = N*N;
    rep(i, N) rep(j, N) {
        int type = stoi(string(1, inputs::t[i][j]), nullptr, 16);
        tiles.push_back(Tile(idx2(i, j), (Tile::Type)type));
        Tile &t = tiles[idx2(i, j)];
        all_edge_num += Tile::edge_num(t.type);
    }
    tiles.push_back(Tile(-1, Tile::Type::BLANK));
    int n = 1000;
    double s = 0;
    double m = 0;
    rep(_, n) {
        double score = solve1::generate_spanning_tree();
        s += score; chmax(m, score);
    }
    print(s / (double)n, m);
    print("sec:", timer.get());
}

int main() {
    cin.tie(0);
    ios::sync_with_stdio(false);
    init();
    return 0;
}
