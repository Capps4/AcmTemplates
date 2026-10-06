#pragma once
#include "Include.hpp"

// SNIPPET BEGIN
namespace _treemap {
using u32 = std::uint32_t;

struct TreePath {
    u32 path[128]; // Only [0, top) is initialized, read or copied.
    int top = 0;

    TreePath() {} // Do not zero the whole path on construction.

    TreePath(const TreePath &b) : top(b.top) {
        std::copy_n(b.path, top, path);
    }

    TreePath &operator=(const TreePath &b) {
        if (this != &b) {
            top = b.top;
            std::copy_n(b.path, top, path);
        }
        return *this;
    }

    u32 get() const {
        return top ? path[top - 1] : 0;
    }

    void push(u32 u) {
        if (top == 128)
            throw std::length_error("TreeMap iterator path overflow");
        path[top++] = u;
    }
};

// Value is stored separately, including native vector<bool> proxies.
// Structural mutations invalidate iterators, references and proxies.
template <class Key, class Value, class Derived>
class MapApi {
    Derived &self() {
        return static_cast<Derived &>(*this);
    }
    const Derived &self() const {
        return static_cast<const Derived &>(*this);
    }

protected:
    void wipe(u32 fst, u32 last) {
        if constexpr (!std::is_trivial_v<Value> or !std::is_trivially_move_assignable_v<Value>)
            for (u32 u = fst; u < last; ++u)
                self().val[u] = Value{};
    }

public:
    bool empty() const {
        return self().size() == 0;
    }

    bool insert(Key key, Value value) {
        return self().template save<0>(key, &value).second;
    }

    bool insertOrAssign(Key key, Value value) {
        return self().template save<1>(key, &value).second;
    }

    decltype(auto) operator[](Key key) {
        return self().val[self().template save<2>(key, nullptr).first];
    }

    std::optional<Value> operator()(const Key &key) const {
        if (u32 u = self().find(key))
            return self().val[u];
        return std::nullopt;
    }

    bool contains(const Key &key) const {
        return self().find(key) != 0;
    }

    Key keyAt(int k) const {
        if (u32 u = self().kth(k))
            return self().key(u);
        throw std::out_of_range("TreeMap keyAt");
    }

    template <bool IsConst>
    class Iterator {
        friend class MapApi;
        using Owner = std::conditional_t<IsConst, const Derived, Derived>;
        Owner *tree;
        typename Derived::Cursor cur{};

        Iterator(Owner *own, bool fst) : tree(own) {
            if (fst)
                tree->head(cur);
        }

    public:
        struct Entry {
            const Key &key;
            std::conditional_t<IsConst, typename std::vector<Value>::const_reference,
                               typename std::vector<Value>::reference>
                value;
        };
        Entry operator*() const {
            u32 u = cur.get();
            return {tree->key(u), tree->val[u]};
        }

        Iterator &operator++() {
            tree->next(cur);
            return *this;
        }

        bool operator==(const Iterator &b) const {
            return tree == b.tree and cur.get() == b.cur.get();
        }

        bool operator!=(const Iterator &b) const {
            return !(*this == b);
        }
    };

    Iterator<false> begin() {
        return {&self(), true};
    }
    Iterator<true> begin() const {
        return {&self(), true};
    }
    Iterator<false> end() {
        return {&self(), false};
    }
    Iterator<true> end() const {
        return {&self(), false};
    }
};

// Online backend: 16B hot node for int keys; payload never enters Node.
template <class Key, class Value, class Compare = std::less<Key>>
class TreeMap : public MapApi<Key, Value, TreeMap<Key, Value, Compare>> {
    friend class MapApi<Key, Value, TreeMap>;
    using Cursor = TreePath;

    struct Node {
        Key key{};
        u32 l = 0, r = 0, sz = 0;
    };
    static_assert(!std::is_same_v<Key, int> or sizeof(Node) == 16, "int Node must be 16 bytes");

    Compare comp;
    std::vector<Node> ns;
    std::vector<Value> val;
    u32 root = 0, free = 0, used = 0;
    bool fix = false;

    const Key &key(u32 u) const {
        return ns[u].key;
    }
    int capacity() const {
        return ns.empty() ? 0 : static_cast<int>(ns.size() - 1);
    }
    void pull(u32 u) {
        ns[u].sz = ns[ns[u].l].sz + ns[ns[u].r].sz + 1;
    }

    template <bool Right>
    u32 rot(u32 u) {
        u32 &son = Right ? ns[u].l : ns[u].r;
        u32 v = son;
        u32 &mid = Right ? ns[v].r : ns[v].l;
        son = mid;
        mid = u;
        ns[v].sz = ns[u].sz;
        pull(u);
        return v;
    }

    u32 bal(u32 u, bool rev) {
        u32 l = ns[u].l, r = ns[u].r;
        if (!rev) {
            if (ns[ns[l].l].sz > ns[r].sz)
                u = rot<true>(u);
            else if (ns[ns[l].r].sz > ns[r].sz) {
                ns[u].l = rot<false>(l);
                u = rot<true>(u);
            } else
                return u;
        } else {
            if (ns[ns[r].r].sz > ns[l].sz)
                u = rot<false>(u);
            else if (ns[ns[r].l].sz > ns[l].sz) {
                ns[u].r = rot<true>(r);
                u = rot<false>(u);
            } else
                return u;
        }
        ns[u].l = bal(ns[u].l, false);
        ns[u].r = bal(ns[u].r, true);
        return bal(bal(u, false), true);
    }

    template <int Mode>
    u32 make(Key &key, Value *value) {
        if (!free and used == static_cast<u32>(capacity())) {
            if (fix)
                throw std::length_error("TreeMap full");
            reserve(std::max(1, 2 * capacity()));
        }
        u32 u = free ? free : used + 1, next = ns[u].l;
        ns[u].key = std::move(key);
        if constexpr (Mode == 2)
            val[u] = Value{};
        else
            val[u] = std::move(*value);
        ns[u].l = ns[u].r = 0;
        ns[u].sz = 1;
        if (free)
            free = next;
        else
            ++used;
        return u;
    }

    void drop(u32 u) {
        this->wipe(u, u + 1);
        ns[u].l = free;
        free = u;
    }

    // Mode: 0 insert, 1 insertOrAssign, 2 subscript.
    template <int Mode>
    u32 put(u32 u, Key &key, Value *value, u32 &dst, bool &add) {
        if (!u) {
            dst = make<Mode>(key, value);
            add = true;
            return dst;
        }
        auto link = [&](auto side) {
            constexpr bool rev = decltype(side)::value;
            u32 son = put<Mode>(rev ? ns[u].r : ns[u].l, key, value, dst, add);
            if (!add)
                return u;
            (rev ? ns[u].r : ns[u].l) = son;
            ++ns[u].sz;
            return bal(u, rev);
        };
        if (comp(key, ns[u].key))
            return link(std::false_type{});
        if (comp(ns[u].key, key))
            return link(std::true_type{});
        dst = u;
        if constexpr (Mode == 1)
            val[u] = std::move(*value);
        return u;
    }

    template <int Mode>
    std::pair<u32, bool> save(Key &key, Value *value) {
        u32 dst = 0;
        bool add = false;
        root = put<Mode>(root, key, value, dst, add);
        return {dst, add};
    }

    u32 take(u32 u, u32 dst) {
        if (!ns[u].l) {
            val[dst] = std::move(val[u]);
            ns[dst].key = std::move(ns[u].key);
            u32 son = ns[u].r;
            drop(u);
            return son;
        }
        ns[u].l = take(ns[u].l, dst);
        --ns[u].sz;
        return u;
    }

    u32 cut(u32 u, const Key &key, bool &del) {
        if (!u)
            return 0;
        if (comp(key, ns[u].key))
            ns[u].l = cut(ns[u].l, key, del);
        else if (comp(ns[u].key, key))
            ns[u].r = cut(ns[u].r, key, del);
        else {
            del = true;
            if (!ns[u].l or !ns[u].r) {
                u32 son = ns[u].l ? ns[u].l : ns[u].r;
                drop(u);
                return son;
            }
            ns[u].r = take(ns[u].r, u);
        }
        if (del)
            --ns[u].sz; // No deletion rebalance.
        return u;
    }

    u32 find(const Key &key) const {
        u32 u = root;
        while (u) {
            if (comp(key, ns[u].key))
                u = ns[u].l;
            else if (comp(ns[u].key, key))
                u = ns[u].r;
            else
                return u;
        }
        return 0;
    }

    u32 kth(int k) const {
        if (k < 0 or k >= size())
            return 0;
        for (u32 u = root; u;) {
            int ls = ns[ns[u].l].sz;
            if (k < ls)
                u = ns[u].l;
            else if (k == ls)
                return u;
            else {
                k -= ls + 1;
                u = ns[u].r;
            }
        }
        return 0;
    }

    void head(Cursor &cur) const {
        for (u32 u = root; u; u = ns[u].l)
            cur.push(u);
    }

    void next(Cursor &cur) const {
        u32 u = cur.get();
        if (ns[u].r) {
            for (u = ns[u].r; u; u = ns[u].l)
                cur.push(u);
            return;
        }
        --cur.top;
        while (cur.top and ns[cur.path[cur.top - 1]].r == u)
            u = cur.path[--cur.top];
    }

public:
    explicit TreeMap(Compare cmp = {}) : comp(std::move(cmp)), ns(1), val(1) {}

    explicit TreeMap(int capacity, Compare cmp = {}) : TreeMap(std::move(cmp)) {
        fix = true;
        reserve(capacity);
    }

    TreeMap(const TreeMap &) = default;

    TreeMap(TreeMap &&b) noexcept(std::is_nothrow_copy_constructible_v<Compare>)
        : comp(b.comp), ns(std::move(b.ns)), val(std::move(b.val)), root(std::exchange(b.root, 0)),
          free(std::exchange(b.free, 0)), used(std::exchange(b.used, 0)),
          fix(std::exchange(b.fix, false)) {
        b.ns.clear();
        b.val.clear();
    }

    TreeMap &operator=(TreeMap b) noexcept(std::is_nothrow_swappable_v<Compare>) {
        using std::swap;
        swap(comp, b.comp);
        swap(root, b.root);
        swap(free, b.free);
        swap(used, b.used);
        swap(fix, b.fix);
        ns.swap(b.ns);
        val.swap(b.val);
        return *this;
    }

    int size() const {
        return root ? static_cast<int>(ns[root].sz) : 0;
    }

    void reserve(int n) {
        if (n < 0)
            throw std::length_error("TreeMap negative capacity");
        if (n <= capacity())
            return;
        size_t old = val.size();
        val.resize(static_cast<size_t>(n) + 1);
        try {
            ns.resize(static_cast<size_t>(n) + 1);
        } catch (...) {
            val.resize(old);
            throw;
        }
    }

    void clear() {
        this->wipe(1, used + 1);
        root = free = used = 0;
    }

    bool erase(const Key &key) {
        bool del = false;
        root = cut(root, key, del);
        return del;
    }

    int rankOf(Key key) const {
        u32 u = root;
        int res = 0;
        while (u) {
            if (comp(ns[u].key, key)) {
                res += ns[ns[u].l].sz + 1;
                u = ns[u].r;
            } else
                u = ns[u].l;
        }
        return res;
    }
};

// Offline backend: only the registered key universe can be written to.
template <class Key, class Value, class Compare = std::less<Key>>
class TreeMapOff : public MapApi<Key, Value, TreeMapOff<Key, Value, Compare>> {
    friend class MapApi<Key, Value, TreeMapOff>;

    struct Cursor {
        u32 id = 0;
        u32 get() const {
            return id;
        }
    };

    Compare comp;
    std::vector<Key> keys;
    std::vector<Value> val{};
    std::vector<u32> bit{};
    std::vector<unsigned char> live{};
    u32 cnt = 0, jump = 0;

    const Key &key(u32 u) const {
        return keys[u - 1];
    }

    size_t rank(const Key &key) const {
        return std::lower_bound(keys.begin(), keys.end(), key, comp) - keys.begin();
    }

    bool has(size_t p, const Key &key) const {
        return p < keys.size() and !comp(key, keys[p]);
    }

    void add(u32 u, int dif) {
        for (u32 i = u; i < bit.size(); i += i & -i)
            bit[i] += dif;
    }

    u32 sum(size_t p) const {
        u32 res = 0;
        for (; p; p -= p & -p)
            res += bit[p];
        return res;
    }

    u32 sel(u32 k) const {
        u32 u = 0;
        for (u32 step = jump; step; step >>= 1) {
            u32 next = u + step;
            if (next < bit.size() and bit[next] <= k) {
                u = next;
                k -= bit[next];
            }
        }
        return u + 1;
    }

    template <int Mode>
    std::pair<u32, bool> save(Key &key, Value *value) {
        size_t p = rank(key);
        if (!has(p, key))
            throw std::out_of_range("TreeMapOff unregistered key");
        u32 u = static_cast<u32>(p + 1);
        if (live[u]) {
            if constexpr (Mode == 1)
                val[u] = std::move(*value);
            return {u, false};
        }
        keys[p] = std::move(key); // An equivalent representative preserves the sorted universe.
        if constexpr (Mode == 2)
            val[u] = Value{};
        else
            val[u] = std::move(*value);
        live[u] = 1;
        add(u, 1);
        ++cnt;
        return {u, true};
    }

    u32 find(const Key &key) const {
        size_t p = rank(key);
        return has(p, key) and live[p + 1] ? p + 1 : 0;
    }

    u32 kth(int k) const {
        return k >= 0 and static_cast<u32>(k) < cnt ? sel(k) : 0;
    }

    void head(Cursor &cur) const {
        next(cur);
    }

    void next(Cursor &cur) const {
        for (size_t u = cur.id + 1; u < live.size(); ++u)
            if (live[u]) {
                cur.id = u;
                return;
            }
        cur.id = 0;
    }

public:
    explicit TreeMapOff(std::vector<Key> vs, Compare cmp = {})
        : comp(std::move(cmp)), keys(std::move(vs)) {
        keys = std::move(keys) | seq::sorted(comp);
        keys.erase(std::unique(keys.begin(), keys.end(),
                               [&](const Key &a, const Key &b) {
                                   return !comp(a, b);
                               }),
                   keys.end());
        val.resize(keys.size() + 1);
        bit.resize(keys.size() + 1);
        live.resize(keys.size() + 1);
        if (!keys.empty())
            for (jump = 1; jump <= keys.size() / 2; jump <<= 1) {
            }
    }

    TreeMapOff(const TreeMapOff &) = default;

    TreeMapOff(TreeMapOff &&b) noexcept(std::is_nothrow_copy_constructible_v<Compare>)
        : comp(b.comp), keys(std::move(b.keys)), val(std::move(b.val)), bit(std::move(b.bit)),
          live(std::move(b.live)), cnt(std::exchange(b.cnt, 0)), jump(std::exchange(b.jump, 0)) {
        b.keys.clear();
        b.val.clear();
        b.bit.clear();
        b.live.clear();
    }

    TreeMapOff &operator=(TreeMapOff b) noexcept(std::is_nothrow_swappable_v<Compare>) {
        using std::swap;
        swap(comp, b.comp);
        swap(cnt, b.cnt);
        swap(jump, b.jump);
        keys.swap(b.keys);
        val.swap(b.val);
        bit.swap(b.bit);
        live.swap(b.live);
        return *this;
    }

    int size() const {
        return static_cast<int>(cnt);
    }

    void clear() {
        this->wipe(1, static_cast<u32>(val.size()));
        std::fill(bit.begin(), bit.end(), 0);
        std::fill(live.begin(), live.end(), 0);
        cnt = 0;
    }

    bool erase(const Key &key) {
        u32 u = find(key);
        if (!u)
            return false;
        this->wipe(u, u + 1);
        live[u] = 0;
        add(u, -1);
        --cnt;
        return true;
    }

    int rankOf(Key key) const {
        return sum(rank(key));
    }
};
}

using _treemap::TreeMap;
using _treemap::TreeMapOff;
