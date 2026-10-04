#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
namespace _trie {

template<class Info, class Keys>
class Trie {
    using Index = int;

public:
    using Key = typename Keys::Key;
    static constexpr int degree = Keys::degree;
    struct Node {
        std::array<Index, degree> son{};
        Info info{};
        const Node& operator[](int d) const { return nds[son[d]]; }
    };

private:
    inline static std::vector<Node> nds;
    Index rt = 0;
    // 非零节点编号 < frozen 时受旧版本保护，>= frozen 时独占；0 表示全树独占。
    mutable Index frozen = 0;

public:
    // 再次初始化使所有旧树失效；0 号节点始终为空节点。
    static void clearInit(std::size_t cap = 0) {
        nds.clear(); nds.reserve(cap + 1); nds.push_back(Node{});
    }
    Trie() { assert(!nds.empty()); }
    Trie(const Trie& b) : rt(b.rt) {
        frozen = b.frozen = rt == 0 ? 0 : nextIndex();
    }
    Trie& operator=(const Trie& b) {
        if (this != &b) {
            rt = b.rt;
            frozen = b.frozen = rt == 0 ? 0 : nextIndex();
        }
        return *this;
    }
    Trie(Trie&& b) noexcept
        : rt(std::exchange(b.rt, 0)), frozen(std::exchange(b.frozen, 0)) {}
    Trie& operator=(Trie&& b) noexcept {
        if (this != &b) {
            rt = std::exchange(b.rt, 0);
            frozen = std::exchange(b.frozen, 0);
        }
        return *this;
    }

    // 对根及路径上的每个节点调用 f(Info&, int dep)；空键只更新根。
    template<class F>
    Trie& modify(Key x, F f) {
        const int len = Keys::len(x);
        rt = writable(rt, frozen);
        walkImpl(rt,
            [&](Index u, int dep) -> int {
                f(nds[u].info, dep);
                return dep == len ? -1 : Keys::symbol(x, dep);
            },
            [&](Index& u, int d) {
                const Index nxt = writable(nds[u].son[d], frozen);
                nds[u].son[d] = nxt;
                u = nxt;
            }
        );
        return *this;
    }

    // f(const Node&, int dep) 返回下一条边；-1 表示结束。
    template<class F>
    void walk(F f) const {
        walkImpl(rt,
            [&](Index u, int dep) -> int { return f(std::as_const(nds[u]), dep); },
            readStep
        );
    }
    // f(const Node&, const Node&, int dep)；两根沿同一条边前进。
    template<class F>
    void walk(const Trie& b, F f) const {
        walkImpl(std::array<Index, 2>{rt, b.rt},
            [&](const auto& us, int dep) -> int {
                return f(std::as_const(nds[us[0]]), std::as_const(nds[us[1]]), dep);
            },
            [](auto& us, int d) {
                readStep(us[0], d); readStep(us[1], d);
            }
        );
    }
    // 四根同路遍历；字段如何组合由 f 决定。
    template<class F>
    void walk(const Trie& b, const Trie& c, const Trie& d, F f) const {
        walkImpl(std::array<Index, 4>{rt, b.rt, c.rt, d.rt},
            [&](const auto& us, int dep) -> int {
                return f(std::as_const(nds[us[0]]), std::as_const(nds[us[1]]),
                         std::as_const(nds[us[2]]), std::as_const(nds[us[3]]), dep);
            },
            [](auto& us, int s) { for (auto& u : us) readStep(u, s); }
        );
    }
    Info query(Key x) const {
        const int len = Keys::len(x);
        const Index u = walkImpl(rt,
            [&](Index u, int dep) -> int {
                return u == 0 || dep == len ? -1 : Keys::symbol(x, dep);
            },
            readStep
        );
        return nds[u].info;
    }

private:
    // 空节点也由 f 决定何时结束；d 必须为 -1 或 [0, degree) 内的编号。
    template<class State, class F, class Step>
    static State walkImpl(State cur, F f, Step step) {
        for (int dep = 0;; ++dep) {
            const int d = f(std::as_const(cur), dep);
            if (d == -1) return cur;
            step(cur, d);
        }
    }
    static void readStep(Index& u, int d) { u = nds[u].son[d]; }
    static Index nextIndex() { return static_cast<Index>(nds.size()); }
    static Index writable(Index u, Index frozen) {
        if (u != 0 && u >= frozen) return u;
        Node cp = nds[u]; // 局部副本不受 vector 扩容影响。
        nds.push_back(std::move(cp));
        return nextIndex() - 1;
    }
};

// 两种包装只负责路径长度与字符/按位编码；键的合法性由调用方保证。
template<int D, char First>
struct StringKeys {
    static_assert(D > 0 && int(static_cast<unsigned char>(First)) + D <= 256, "Invalid alphabet");
    static constexpr int degree = D;
    using Key = std::string_view;
    static int len(Key x) { return static_cast<int>(x.size()); }
    static int symbol(Key x, int dep) {
        return int(static_cast<unsigned char>(x[dep])) - int(static_cast<unsigned char>(First));
    }
};

template<class UInt, int Bits>
struct BinaryKeys {
    static_assert(std::is_integral_v<UInt> && std::is_unsigned_v<UInt>, "Unsigned integer required");
    static_assert(std::numeric_limits<UInt>::radix == 2, "Binary integer required");
    static_assert(1 <= Bits && Bits <= std::numeric_limits<UInt>::digits, "Invalid bit width");
    static constexpr int degree = 2;
    using Key = UInt;
    static int len(Key) { return Bits; }
    static int symbol(Key x, int dep) { return int((x >> (Bits - 1 - dep)) & Key{1}); }
}; }

template<class Info, int D = 26, char First = 'a'>
using StringTrie = _trie::Trie<Info, _trie::StringKeys<D, First>>;

template<class Info, class UInt = unsigned, int Bits = std::numeric_limits<UInt>::digits>
using BinaryTrie = _trie::Trie<Info, _trie::BinaryKeys<UInt, Bits>>;

// replace me: prefix count and terminal count.
struct Info {
    int count = 0;
    int end = 0;
};
