#pragma once
#include "../../../../Headers/Headers.hpp"
using u64 = unsigned long long;
using u32 = unsigned int;

// Competition use: one translation unit; the modulus is a compile-time constant.
namespace _hashmap {
    constexpr u64 mix(u64 x) {
        return x ^ (x << 13) ^ (x >> 3);
    }
    constexpr u64 fold(u64 x, const char* str, int idx = 0) {
        return str[idx] == '\0' ? x : fold(mix(x ^ u64(str[idx])), str, idx + 1);
    }
    constexpr u64 rng(u64 seed) {
        return fold(seed ^ __LINE__, __TIME__ __TIMESTAMP__ __FILE__);
    }
    constexpr u64 sqr(u64 seed) {
        return seed - rng(seed) % u64(std::sqrt(seed) + 1) + 1;
    }

template <class Val, int Mod, int Capa> class Impl;

template <class Val, int Mod, int Capa>
class Pool {
    friend class Impl<Val, Mod, Capa>;
    static_assert(Mod > 0 and Capa > 0 and Capa < std::numeric_limits<int>::max(), "Invalid pool size");
    static_assert(std::is_trivially_destructible<Val>::value or
                  (std::is_nothrow_default_constructible<Val>::value and
                   std::is_nothrow_move_assignable<Val>::value and
                   std::is_nothrow_destructible<Val>::value), "Val must support noexcept reset");

    int head[Mod]{}, next[Capa + 1]{}, link[Capa + 1]{};
    u64 key[Capa + 1]{};
    u32 own[Capa + 1]{};
    Val val[Capa + 1]{};
    int top = 0, act = 0;
    u32 ser = 0;

    int hash(u64 x, int off) const noexcept {
        return int((x + u64(off)) % Mod);
    }
    u32 take() {
        if (ser == std::numeric_limits<u32>::max() or act == std::numeric_limits<int>::max())
            throw std::overflow_error("HashMap own limit");
        ++act;
        return ++ser;
    }
    void wipe(std::true_type) noexcept {}
    void wipe(std::false_type) noexcept {
        for (int i = 1; i <= top; ++i) val[i] = Val{};
    }
    void reset() noexcept {
        wipe(std::is_trivially_destructible<Val>());
        if (top > (Mod - 1) / 32) std::fill(head, head + Mod, 0); // Mod <= 32 * top.
        else
            for (int i = 1; i <= top; ++i)
            if (next[i] < 0) head[-next[i] - 1] = 0;
        top = 0;
        ser = 0;
    }
    u32 init() {
        if (act == 1) reset();
        if (ser == std::numeric_limits<u32>::max())
            throw std::overflow_error("HashMap own limit");
        return ++ser;
    }
    void drop() noexcept {
        if (--act == 0) reset();
    }
    int find(u64 x, u32 id, int h, int beg, int last) const noexcept {
        for (int i = head[h]; i > 0; i = next[i])
            if (key[i] == x and (u32(i - beg) < u32(last - beg) or own[i] == id)) return i;
        return 0;
    }
    int add(u64 x, u32 id, int h) {
        if (top == Capa)
            throw std::length_error("HashMap pool full");
        int i = top + 1;
        val[i] = Val{}; // Publish only after value initialization succeeds.
        key[i] = x;
        own[i] = id;
        next[i] = head[h] ? head[h] : -h - 1; // Chain tail also records its hash.
        return head[h] = top = i;
    }
    void join(int beg, int last, int nxt) noexcept {
        // One-node run: -next. Longer run: length at beg, next at last.
        if (beg == last) link[beg] = -nxt;
        else {
            link[beg] = last - beg + 1;
            link[last] = nxt;
        }
    }
    int end(int beg) const noexcept {
        return beg + (link[beg] < 0 ? 1 : link[beg]);
    }
    int step(int last) const noexcept {
        return link[last] < 0 ? -link[last] : link[last];
    }
};

template <class Val, int Mod, int Capa>
class Impl {
    static Pool<Val, Mod, Capa>& get() {
        static Pool<Val, Mod, Capa> pool;
        return pool;
    }
    u32 id;
    int off, fst = 0, beg = 0, last = 0; // Tail run: [beg, last).

    void swap(Impl& rhs) noexcept {
        std::swap(id, rhs.id);
        std::swap(off, rhs.off);
        std::swap(fst, rhs.fst);
        std::swap(beg, rhs.beg);
        std::swap(last, rhs.last);
    }
    Val& insert(u64 x, int h) {
        auto& pool = get();
        int i = pool.add(x, id, h);
        if (last != i) {
            if (fst)
                pool.join(beg, last - 1, i);
            else
                fst = i;
            beg = i;
        }
        last = i + 1;
        return pool.val[i];
    }
public:
    struct Iterator {
        int pos, stop, tail, end;
        bool flat;
        Iterator& operator++() noexcept {
            ++pos;
            if (!flat and pos == stop and pos != end) {
                auto& pool = get();
                pos = pool.step(pos - 1);
                stop = pos == tail ? end : pool.end(pos);
            }
            return *this;
        }
        bool operator==(Iterator rhs) const noexcept { return pos == rhs.pos; }
        bool operator!=(Iterator rhs) const noexcept { return pos != rhs.pos; }
        std::pair<u64, Val> operator*() const {
            auto& pool = get();
            return {pool.key[pos], pool.val[pos]};
        }
    };

    Impl() : id(get().take()), off(int(mix(id) % Mod)) {}
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl(Impl&& rhs) : Impl() {
        swap(rhs);
    }
    Impl& operator=(Impl&& rhs) {
        if (this != &rhs) {
            clear();
            swap(rhs);
        }
        return *this;
    }
    ~Impl() {
        get().drop();
    }

    Iterator begin() const noexcept {
        auto& pool = get();
        int end = last;
        return {fst, fst == beg ? end : pool.end(fst), beg, end, fst == beg};
    }
    Iterator end() const noexcept {
        int end = last;
        return {end, end, end, end, true};
    }
    void clear() {
        id = get().init();
        off = int(mix(id) % Mod);
        fst = beg = last = 0;
    }
    const Val& operator()(u64 x) const {
        auto& pool = get();
        int i = pool.find(x, id, pool.hash(x, off), beg, last);
        if (i)
            return pool.val[i];
        static const Val empty{};
        return empty;
    }
    Val& operator[](u64 x) {
        auto& pool = get();
        int h = pool.hash(x, off), i = pool.find(x, id, h, beg, last);
        return i ? pool.val[i] : insert(x, h);
    }
};

} // namespace _hashmap

template <class Val, int N = int(5E6), int Capa = N + 3>
using HashMap = _hashmap::Impl<Val, _hashmap::sqr(N), Capa>;

