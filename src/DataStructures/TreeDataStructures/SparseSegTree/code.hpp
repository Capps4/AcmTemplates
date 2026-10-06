#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
template <class Info, class Coord>
class SparseSegTree {
    static_assert(std::is_integral_v<Coord>, "Coord must be integral");
    using Index = int;
    struct Node {
        std::array<Index, 2> ch{};
        Info info{};
    };
    struct Term {
        Index rt = 0;
        bool neg = false;
    };

    inline static std::vector<Node> nds;
    inline static Coord lo{}, up{};
    Index rt = 0;
    // 非零节点编号 < frz 时受快照保护，>= frz 时独占；0 表示全树独占。
    mutable Index frz = 0;

    static Info add(const Info &a, const Info &b, Coord) {
        return a + b;
    }

public:
    // 只保存根编号和正负号。带减法的视图才要求 Info 支持 operator-。
    template <std::size_t N, bool Signed = false>
    class View {
        friend class SparseSegTree;
        template <std::size_t, bool>
        friend class View;
        std::array<Term, N> rs{};

        Info info() const {
            Info res{};
            for (const auto &t : rs) {
                const Info &val = at(t.rt);
                if constexpr (Signed) {
                    res = t.neg ? res - val : res + val;
                } else {
                    res = res + val;
                }
            }
            return res;
        }
        bool empty() const {
            for (const auto &t : rs)
                if (t.rt != 0)
                    return false;
            return true;
        }
        View ch(int d) const {
            View res = *this;
            for (auto &t : res.rs)
                t.rt = nds[t.rt].ch[d];
            return res;
        }
        template <bool Sub, std::size_t M, bool S>
        auto cat(const View<M, S> &b) const {
            View<N + M, Signed or S or Sub> res;
            for (std::size_t i = 0; i < N; ++i)
                res.rs[i] = rs[i];
            for (std::size_t i = 0; i < M; ++i) {
                res.rs[N + i] = b.rs[i];
                res.rs[N + i].neg ^= Sub;
            }
            return res;
        }

    public:
        template <std::size_t M, bool S>
        auto operator+(const View<M, S> &b) const {
            return cat<false>(b);
        }
        template <std::size_t M, bool S>
        auto operator-(const View<M, S> &b) const {
            return cat<true>(b);
        }
        auto operator+(const SparseSegTree &b) const {
            return *this + b.view();
        }
        auto operator-(const SparseSegTree &b) const {
            return *this - b.view();
        }

        Info query(Coord l, Coord r) const {
            test(l, r);
            assert(l < r);
            return qry(*this, lo, up, l, r);
        }
        template <class F>
        std::optional<Coord> first(Coord l, Coord r, F f) const {
            return find<false>(l, r, f);
        }
        template <class F>
        std::optional<Coord> last(Coord l, Coord r, F f) const {
            return find<true>(l, r, f);
        }

    private:
        template <bool Rev, class F>
        std::optional<Coord> find(Coord l, Coord r, F &f) const {
            test(l, r);
            Info acc{};
            return seek<Rev>(*this, lo, up, l, r, acc, f);
        }
    };

    // 再次初始化使所有旧树和视图失效；up-lo 必须能用 Coord 表示。
    static void clearInit(Coord lo, Coord up, std::size_t cap = 0) {
        assert(lo < up);
        nds.clear();
        nds.reserve(cap + 1);
        nds.push_back(Node{});
        SparseSegTree::lo = lo;
        SparseSegTree::up = up;
    }
    SparseSegTree() {
        assert(!nds.empty());
    }
    SparseSegTree(const SparseSegTree &b) : rt(b.rt) {
        // 同时冻结源树与副本；nextIndex() 是下一个新节点的编号。
        frz = b.frz = rt == 0 ? 0 : next();
    }
    SparseSegTree &operator=(const SparseSegTree &b) {
        if (this != &b) {
            rt = b.rt;
            frz = b.frz = rt == 0 ? 0 : next();
        }
        return *this;
    }
    SparseSegTree(SparseSegTree &&b) noexcept
        : rt(std::exchange(b.rt, 0)), frz(std::exchange(b.frz, 0)) {}
    SparseSegTree &operator=(SparseSegTree &&b) noexcept {
        if (this != &b) {
            rt = std::exchange(b.rt, 0);
            frz = std::exchange(b.frz, 0);
        }
        return *this;
    }

    // 更新自己；独占节点原地修改，共享节点自动 COW。
    template <class F>
    SparseSegTree &modify(Coord x, F f) {
        assert(lo <= x and x < up);
        auto g = [&](const Info &v) -> Info {
            if constexpr (std::is_same_v<std::decay_t<F>, Info>)
                return f;
            else
                return f(v);
        };
        rt = frz == 0 ? put<InPlace>(rt, lo, up, x, g) : edit(rt, lo, up, x, frz, g);
        return *this;
    }

    // 按值接收右树：传 b 保留它，传 std::move(b) 消费它。
    template <class F = decltype(&add)>
    SparseSegTree &merge(SparseSegTree b, F f = &add) {
        // 接入的分支可能来自两边，取较大边界保护它们的历史版本。
        frz = std::max(frz, b.frz);
        rt = frz == 0 ? join<InPlace>(rt, b.rt, lo, up, f) : meld(rt, b.rt, lo, up, frz, f);
        return *this;
    }

    View<1> view() const {
        frz = rt == 0 ? 0 : next(); // 冻结快照中所有现有节点。
        return peek();
    }
    Info query(Coord l, Coord r) const {
        return peek().query(l, r);
    }
    template <class F>
    std::optional<Coord> first(Coord l, Coord r, F f) const {
        return peek().first(l, r, std::move(f));
    }
    template <class F>
    std::optional<Coord> last(Coord l, Coord r, F f) const {
        return peek().last(l, r, std::move(f));
    }
    friend auto operator+(const SparseSegTree &a, const SparseSegTree &b) {
        return a.view() + b.view();
    }
    friend auto operator-(const SparseSegTree &a, const SparseSegTree &b) {
        return a.view() - b.view();
    }
    template <std::size_t N, bool S>
    friend auto operator+(const SparseSegTree &a, const View<N, S> &b) {
        return a.view() + b;
    }
    template <std::size_t N, bool S>
    friend auto operator-(const SparseSegTree &a, const View<N, S> &b) {
        return a.view() - b;
    }

private:
    View<1> peek() const {
        View<1> res;
        res.rs[0].rt = rt;
        return res;
    }
    static Index next() {
        return static_cast<Index>(nds.size());
    }
    static void test(Coord l, Coord r) {
        assert(lo <= l and l <= r and r <= up);
        (void)l;
        (void)r;
    }
    static const Info &at(Index rt) {
        return nds[rt].info;
    }
    class InPlace {};
    class Copy {};
    template <class Mode>
    static Index own(Index rt) {
        if constexpr (std::is_same_v<Mode, InPlace>) {
            if (rt != 0)
                return rt;
        }
        // 不将 vector 内的引用跨过可能扩容的调用保存。
        Node cp = nds[rt]; // 0 号节点的 info 为 Info{}，孩子也都为 0。
        nds.push_back(std::move(cp));
        return static_cast<Index>(nds.size() - 1);
    }
    static void pull(Index rt) {
        nds[rt].info = at(nds[rt].ch[0]) + at(nds[rt].ch[1]);
    }
    template <class Mode, class F>
    static Index put(Index rt, Coord l, Coord r, Coord x, F &f) {
        rt = own<Mode>(rt);
        if (r - l == 1) {
            nds[rt].info = f(nds[rt].info);
            return rt;
        }
        const Coord m = l + (r - l) / 2;
        const int d = x < m ? 0 : 1;
        const Index nxt = put<Mode>(nds[rt].ch[d], d == 0 ? l : m, d == 0 ? m : r, x, f);
        nds[rt].ch[d] = nxt;
        pull(rt);
        return rt;
    }
    template <class Mode, class F>
    static Index join(Index a, Index b, Coord l, Coord r, F &f) {
        if (a == 0 or b == 0)
            return a == 0 ? b : a;
        const Index res = own<Mode>(a);
        if (r - l == 1) {
            nds[res].info = f(nds[a].info, nds[b].info, l);
            return res;
        }
        const Coord m = l + (r - l) / 2;
        const Index lc = join<Mode>(nds[res].ch[0], nds[b].ch[0], l, m, f);
        const Index rc = join<Mode>(nds[res].ch[1], nds[b].ch[1], m, r, f);
        nds[res].ch = {lc, rc};
        pull(res);
        return res;
    }

    template <class F>
    static Index edit(Index rt, Coord l, Coord r, Coord x, Index frz, F &f) {
        // 遇到受保护节点后直接复制整条路径，省去后续逐层判断。
        if (rt != 0 and rt < frz)
            return put<Copy>(rt, l, r, x, f);
        rt = own<InPlace>(rt);
        if (r - l == 1) {
            nds[rt].info = f(nds[rt].info);
            return rt;
        }
        const Coord m = l + (r - l) / 2;
        const int d = x < m ? 0 : 1;
        const Index nxt = edit(nds[rt].ch[d], d == 0 ? l : m, d == 0 ? m : r, x, frz, f);
        nds[rt].ch[d] = nxt;
        pull(rt);
        return rt;
    }
    template <class F>
    static Index meld(Index a, Index b, Coord l, Coord r, Index frz, F &f) {
        if (a == 0 or b == 0)
            return a == 0 ? b : a;
        if (a < frz)
            return join<Copy>(a, b, l, r, f);
        if (r - l == 1) {
            nds[a].info = f(nds[a].info, nds[b].info, l);
            return a;
        }
        const Coord m = l + (r - l) / 2;
        const Index lc = meld(nds[a].ch[0], nds[b].ch[0], l, m, frz, f);
        const Index rc = meld(nds[a].ch[1], nds[b].ch[1], m, r, frz, f);
        nds[a].ch = {lc, rc};
        pull(a);
        return a;
    }
    template <std::size_t N, bool S>
    static Info qry(const View<N, S> &view, Coord l, Coord r, Coord ql, Coord qr) {
        if (view.empty() or (ql <= l and r <= qr))
            return view.info();
        const Coord m = l + (r - l) / 2;
        if (qr <= m)
            return qry(view.ch(0), l, m, ql, qr);
        if (m <= ql)
            return qry(view.ch(1), m, r, ql, qr);
        return qry(view.ch(0), l, m, ql, qr) + qry(view.ch(1), m, r, ql, qr);
    }
    template <bool Rev, std::size_t N, bool S, class F>
    static std::optional<Coord> seek(const View<N, S> &view, Coord l, Coord r, Coord ql, Coord qr,
                                     Info &acc, F &f) {
        if (qr <= l or r <= ql or ql == qr)
            return std::nullopt;
        if (ql <= l and r <= qr) {
            const Info val = view.info();
            Info cur = Rev ? val + acc : acc + val;
            if (!f(cur)) {
                acc = std::move(cur); // 整块跳过，保留已经累计的信息。
                return std::nullopt;
            }
            if (r - l == 1)
                return l;
        }
        const Coord m = l + (r - l) / 2;
        for (int s = 0; s < 2; ++s) {
            const int d = Rev ? 1 - s : s;
            auto res = seek<Rev>(view.ch(d), d == 0 ? l : m, d == 0 ? m : r, ql, qr, acc, f);
            if (res)
                return res;
        }
        return std::nullopt;
    }
};

// replace me: count and sum, with signed combinations for views.
struct Info {
    long long count = 0;
    long long sum = 0;

    Info operator+(const Info &v) const {
        return {count + v.count, sum + v.sum};
    }

    Info operator-(const Info &v) const {
        return {count - v.count, sum - v.sum};
    }
};
