#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
#include <type_traits>
#include <utility>

// SNIPPET BEGIN
namespace _hashmap {
using u64 = unsigned long long;
using u32 = unsigned int;

// Competition use: one translation unit; GCC folds sqrt in GNU++17.
constexpr u64 chaos(u64 x) {
    return x ^ (x << 13) ^ (x >> 3);
}

constexpr u64 filter(u64 x, const char *str, int idx = 0) {
    for (; str[idx] != '\0'; ++idx)
        x = chaos(x ^ u64(str[idx]));
    return x;
}

constexpr u64 rng(u64 seed) {
    return filter(seed ^ __LINE__, __TIME__ __TIMESTAMP__ __FILE__);
}

constexpr u64 rngSubSqrt(u64 seed) {
    return seed - rng(seed) % u64(std::sqrt(seed) + 1) + 1;
}

template <class Val, int Mod, int Capa>
class HashMapImpl;

template <class Val, int Mod, int Capa>
class HashPool {
    friend class HashMapImpl<Val, Mod, Capa>;
    static_assert(Mod > 0 && Capa > 0, "Invalid pool size");
    static_assert(std::is_trivially_destructible_v<Val> || noexcept(std::declval<Val &>() = Val{}),
                  "Val reset must not throw");

    int head[Mod]{}, next[Capa + 1]{}, link[Capa + 1]{};
    u64 key[Capa + 1]{};
    u32 owner[Capa + 1]{};
    Val val[Capa + 1]{};
    int top = 0, active = 0;
    u32 serial = 0;

    int bucket(u64 x, int shift) const noexcept { return int((x + u64(shift)) % Mod); }

    u32 acquire() {
        ++active;
        return ++serial;
    }

    void resetPool() noexcept {
        if constexpr (!std::is_trivially_destructible_v<Val>)
            for (int i = 1; i <= top; ++i)
                val[i] = Val{};
        if (top > (Mod - 1) / 32)
            std::fill(head, head + Mod, 0); // Mod <= 32 * top.
        else
            for (int i = 1; i <= top; ++i)
                if (next[i] < 0) head[-next[i] - 1] = 0;
        top = 0;
        serial = 0;
    }

    u32 renew() {
        if (active == 1) resetPool();
        return ++serial;
    }

    void release() noexcept {
        if (--active == 0) resetPool();
    }

    int find(u64 x, u32 id, int h, int start, int last) const noexcept {
        for (int i = head[h]; i > 0; i = next[i])
            if (key[i] == x && (u32(i - start) < u32(last - start) || owner[i] == id)) return i;
        return 0;
    }

    int add(u64 x, u32 id, int h) {
        int i = top + 1;
        val[i] = Val{}; // Publish only after value initialization succeeds.
        key[i] = x;
        owner[i] = id;
        next[i] = head[h] ? head[h] : -h - 1; // Chain tail also records its bucket.
        return head[h] = top = i;
    }

    void join(int start, int last, int nextRun) noexcept {
        // One-node run: -next. Longer run: length at start, next at last.
        if (start == last)
            link[start] = -nextRun;
        else {
            link[start] = last - start + 1;
            link[last] = nextRun;
        }
    }

    int runEnd(int start) const noexcept { return start + (link[start] < 0 ? 1 : link[start]); }

    int runNext(int last) const noexcept { return link[last] < 0 ? -link[last] : link[last]; }
};

template <class Val, int Mod, int Capa>
class HashMapImpl {
    static HashPool<Val, Mod, Capa> &getPool() {
        static HashPool<Val, Mod, Capa> pool;
        return pool;
    }

    u32 id;
    int shift, first = 0, start = 0, last = 0; // Tail run: [start, last).

    void swap(HashMapImpl &other) noexcept {
        std::swap(id, other.id);
        std::swap(shift, other.shift);
        std::swap(first, other.first);
        std::swap(start, other.start);
        std::swap(last, other.last);
    }

    Val &insert(u64 x, int h) {
        auto &pool = getPool();
        int i = pool.add(x, id, h);
        if (last != i) {
            if (first)
                pool.join(start, last - 1, i);
            else
                first = i;
            start = i;
        }
        last = i + 1;
        return pool.val[i];
    }

public:
    struct Iterator {
        int pos, stop, tail, end;
        bool flat;

        Iterator &operator++() noexcept {
            ++pos;
            if (!flat && pos == stop && pos != end) {
                auto &pool = getPool();
                pos = pool.runNext(pos - 1);
                stop = pos == tail ? end : pool.runEnd(pos);
            }
            return *this;
        }

        bool operator==(Iterator other) const noexcept { return pos == other.pos; }

        bool operator!=(Iterator other) const noexcept { return pos != other.pos; }

        std::pair<u64, Val> operator*() const {
            auto &pool = getPool();
            return {pool.key[pos], pool.val[pos]};
        }
    };

    HashMapImpl() : id(getPool().acquire()), shift(int(chaos(id) % Mod)) {}

    HashMapImpl(const HashMapImpl &) = delete;
    HashMapImpl &operator=(const HashMapImpl &) = delete;

    HashMapImpl(HashMapImpl &&other) : HashMapImpl() { swap(other); }

    HashMapImpl &operator=(HashMapImpl &&other) {
        if (this != &other) {
            clear();
            swap(other);
        }
        return *this;
    }

    ~HashMapImpl() { getPool().release(); }

    Iterator begin() const noexcept {
        auto &pool = getPool();
        int end = last;
        return {first, first == start ? end : pool.runEnd(first), start, end, first == start};
    }

    Iterator end() const noexcept { return {last, last, last, last, true}; }

    void clear() {
        id = getPool().renew();
        shift = int(chaos(id) % Mod);
        first = start = last = 0;
    }

    std::optional<Val> operator()(u64 x) const {
        auto &pool = getPool();
        int i = pool.find(x, id, pool.bucket(x, shift), start, last);
        if (i) return pool.val[i];
        return std::nullopt;
    }

    Val &operator[](u64 x) {
        auto &pool = getPool();
        int h = pool.bucket(x, shift), i = pool.find(x, id, h, start, last);
        return i ? pool.val[i] : insert(x, h);
    }
};
} // namespace _hashmap

template <class Val, int N = int(5E6), int Capa = N + 3>
using HashMap = _hashmap::HashMapImpl<Val, _hashmap::rngSubSqrt(N), Capa>;
