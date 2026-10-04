#pragma once
#include "../ListHelper/Final.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// SNIPPET BEGIN
namespace _treemap {
using u32 = std::uint32_t;

struct TreePath {
    u32 path[128]; // Only [0, top) is initialized, read or copied.
    int top = 0;

    TreePath() {} // Do not zero the whole path on construction.

    TreePath(const TreePath& other) : top(other.top) {
        std::copy_n(other.path, top, path);
    }

    TreePath& operator=(const TreePath& other) {
        if (this != &other) {
            top = other.top;
            std::copy_n(other.path, top, path);
        }
        return *this;
    }

    u32 current() const { return top ? path[top - 1] : 0; }

    void push(u32 u) {
        if (top == 128)
            throw std::length_error("TreeMap iterator path overflow");
        path[top++] = u;
    }
};

// Value is stored separately, including native vector<bool> proxies.
// Structural mutations invalidate iterators, references and proxies.
template<class Key, class Value, class Derived>
class MapApi {
    Derived& self() { return static_cast<Derived&>(*this); }
    const Derived& self() const { return static_cast<const Derived&>(*this); }

protected:
    void resetValues(u32 first, u32 last) {
        if constexpr (!std::is_trivial_v<Value> || !std::is_trivially_move_assignable_v<Value>)
            for (u32 u = first; u < last; ++u)
                self().values[u] = Value{};
    }

public:
    bool empty() const { return self().size() == 0; }

    bool insert(Key key, Value value) {
        return self().template write<0>(key, &value).second;
    }

    bool insertOrAssign(Key key, Value value) {
        return self().template write<1>(key, &value).second;
    }

    decltype(auto) operator[](Key key) {
        return self().values[self().template write<2>(key, nullptr).first];
    }

    std::optional<Value> operator()(const Key& key) const {
        if (u32 u = self().findIndex(key))
            return self().values[u];
        return std::nullopt;
    }

    bool contains(const Key& key) const { return self().findIndex(key) != 0; }

    Key keyAt(int k) const {
        if (u32 u = self().orderIndex(k))
            return self().keyFromIndex(u);
        throw std::out_of_range("TreeMap keyAt");
    }

    template<bool IsConst>
    class Iterator {
        friend class MapApi;
        using Owner = std::conditional_t<IsConst, const Derived, Derived>;
        Owner* tree;
        typename Derived::Cursor cursor;

        Iterator(Owner* owner, bool first) : tree(owner) {
            if (first)
                tree->beginCursor(cursor);
        }

    public:
        struct Entry {
            const Key& key;
            std::conditional_t<IsConst, typename std::vector<Value>::const_reference,
                                       typename std::vector<Value>::reference> value;
        };
        Entry operator*() const {
            u32 u = cursor.current();
            return {tree->keyFromIndex(u), tree->values[u]};
        }

        Iterator& operator++() {
            tree->nextCursor(cursor);
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return tree == other.tree && cursor.current() == other.cursor.current();
        }

        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    Iterator<false> begin() { return {&self(), true}; }
    Iterator<true> begin() const { return {&self(), true}; }
    Iterator<false> end() { return {&self(), false}; }
    Iterator<true> end() const { return {&self(), false}; }
};

// Online backend: 16B hot node for int keys; payload never enters Node.
template<class Key, class Value, class Compare = std::less<Key>>
class TreeMap : public MapApi<Key, Value, TreeMap<Key, Value, Compare>> {
    friend class MapApi<Key, Value, TreeMap>;
    using Cursor = TreePath;

    struct Node {
        Key key{};
        u32 l = 0, r = 0, sz = 0;
    };
    static_assert(!std::is_same_v<Key, int> || sizeof(Node) == 16, "int Node must be 16 bytes");

    Compare compare;
    std::vector<Node> nodes;
    std::vector<Value> values;
    u32 root = 0, freeHead = 0, used = 0;
    bool fixedCapacity = false;

    const Key& keyFromIndex(u32 u) const { return nodes[u].key; }
    int capacity() const { return nodes.empty() ? 0 : static_cast<int>(nodes.size() - 1); }
    void pull(u32 u) { nodes[u].sz = nodes[nodes[u].l].sz + nodes[nodes[u].r].sz + 1; }

    template<bool Right>
    u32 rotate(u32 u) {
        u32& child = Right ? nodes[u].l : nodes[u].r;
        u32 v = child;
        u32& inner = Right ? nodes[v].r : nodes[v].l;
        child = inner;
        inner = u;
        nodes[v].sz = nodes[u].sz;
        pull(u);
        return v;
    }

    u32 maintain(u32 u, bool right) {
        u32 l = nodes[u].l, r = nodes[u].r;
        if (!right) {
            if (nodes[nodes[l].l].sz > nodes[r].sz)
                u = rotate<true>(u);
            else if (nodes[nodes[l].r].sz > nodes[r].sz) {
                nodes[u].l = rotate<false>(l);
                u = rotate<true>(u);
            } else
                return u;
        } else {
            if (nodes[nodes[r].r].sz > nodes[l].sz)
                u = rotate<false>(u);
            else if (nodes[nodes[r].l].sz > nodes[l].sz) {
                nodes[u].r = rotate<true>(r);
                u = rotate<false>(u);
            } else
                return u;
        }
        nodes[u].l = maintain(nodes[u].l, false);
        nodes[u].r = maintain(nodes[u].r, true);
        return maintain(maintain(u, false), true);
    }

    template<int Mode>
    u32 allocate(Key& key, Value* value) {
        if (!freeHead && used == static_cast<u32>(capacity())) {
            if (fixedCapacity)
                throw std::length_error("TreeMap full");
            reserve(std::max(1, 2 * capacity()));
        }
        u32 u = freeHead ? freeHead : used + 1, next = nodes[u].l;
        nodes[u].key = std::move(key);
        if constexpr (Mode == 2)
            values[u] = Value{};
        else
            values[u] = std::move(*value);
        nodes[u].l = nodes[u].r = 0;
        nodes[u].sz = 1;
        if (freeHead)
            freeHead = next;
        else
            ++used;
        return u;
    }

    void recycle(u32 u) {
        this->resetValues(u, u + 1);
        nodes[u].l = freeHead;
        freeHead = u;
    }

    // Mode: 0 insert, 1 insertOrAssign, 2 subscript.
    template<int Mode>
    u32 insertNode(u32 u, Key& key, Value* value, u32& target, bool& added) {
        if (!u) {
            target = allocate<Mode>(key, value);
            added = true;
            return target;
        }
        auto insertSide = [&](auto side) {
            constexpr bool right = decltype(side)::value;
            u32 child = insertNode<Mode>(right ? nodes[u].r : nodes[u].l, key, value, target, added);
            if (!added)
                return u;
            (right ? nodes[u].r : nodes[u].l) = child;
            ++nodes[u].sz;
            return maintain(u, right);
        };
        if (compare(key, nodes[u].key))
            return insertSide(std::false_type{});
        if (compare(nodes[u].key, key))
            return insertSide(std::true_type{});
        target = u;
        if constexpr (Mode == 1)
            values[u] = std::move(*value);
        return u;
    }

    template<int Mode>
    std::pair<u32, bool> write(Key& key, Value* value) {
        u32 target = 0;
        bool added = false;
        root = insertNode<Mode>(root, key, value, target, added);
        return {target, added};
    }

    u32 extractMin(u32 u, u32 target) {
        if (!nodes[u].l) {
            values[target] = std::move(values[u]);
            nodes[target].key = std::move(nodes[u].key);
            u32 child = nodes[u].r;
            recycle(u);
            return child;
        }
        nodes[u].l = extractMin(nodes[u].l, target);
        --nodes[u].sz;
        return u;
    }

    u32 eraseNode(u32 u, const Key& key, bool& erased) {
        if (!u)
            return 0;
        if (compare(key, nodes[u].key))
            nodes[u].l = eraseNode(nodes[u].l, key, erased);
        else if (compare(nodes[u].key, key))
            nodes[u].r = eraseNode(nodes[u].r, key, erased);
        else {
            erased = true;
            if (!nodes[u].l || !nodes[u].r) {
                u32 child = nodes[u].l ? nodes[u].l : nodes[u].r;
                recycle(u);
                return child;
            }
            nodes[u].r = extractMin(nodes[u].r, u);
        }
        if (erased)
            --nodes[u].sz; // No deletion rebalance.
        return u;
    }

    u32 findIndex(const Key& key) const {
        u32 u = root;
        while (u) {
            if (compare(key, nodes[u].key))
                u = nodes[u].l;
            else if (compare(nodes[u].key, key))
                u = nodes[u].r;
            else
                return u;
        }
        return 0;
    }

    u32 orderIndex(int k) const {
        if (k < 0 || k >= size())
            return 0;
        for (u32 u = root; u;) {
            int leftSize = nodes[nodes[u].l].sz;
            if (k < leftSize)
                u = nodes[u].l;
            else if (k == leftSize)
                return u;
            else {
                k -= leftSize + 1;
                u = nodes[u].r;
            }
        }
        return 0;
    }

    void beginCursor(Cursor& cursor) const {
        for (u32 u = root; u; u = nodes[u].l)
            cursor.push(u);
    }

    void nextCursor(Cursor& cursor) const {
        u32 u = cursor.current();
        if (nodes[u].r) {
            for (u = nodes[u].r; u; u = nodes[u].l)
                cursor.push(u);
            return;
        }
        --cursor.top;
        while (cursor.top && nodes[cursor.path[cursor.top - 1]].r == u)
            u = cursor.path[--cursor.top];
    }

public:
    explicit TreeMap(Compare cmp = {}) : compare(std::move(cmp)), nodes(1), values(1) {}

    explicit TreeMap(int capacity, Compare cmp = {}) : TreeMap(std::move(cmp)) {
        fixedCapacity = true;
        reserve(capacity);
    }

    TreeMap(const TreeMap&) = default;

    TreeMap(TreeMap&& other) noexcept(std::is_nothrow_copy_constructible_v<Compare>)
        : compare(other.compare),
          nodes(std::move(other.nodes)),
          values(std::move(other.values)),
          root(std::exchange(other.root, 0)),
          freeHead(std::exchange(other.freeHead, 0)),
          used(std::exchange(other.used, 0)),
          fixedCapacity(std::exchange(other.fixedCapacity, false)) {
        other.nodes.clear();
        other.values.clear();
    }

    TreeMap& operator=(TreeMap other) noexcept(std::is_nothrow_swappable_v<Compare>) {
        using std::swap;
        swap(compare, other.compare);
        swap(root, other.root);
        swap(freeHead, other.freeHead);
        swap(used, other.used);
        swap(fixedCapacity, other.fixedCapacity);
        nodes.swap(other.nodes);
        values.swap(other.values);
        return *this;
    }

    int size() const { return root ? static_cast<int>(nodes[root].sz) : 0; }

    void reserve(int n) {
        if (n < 0)
            throw std::length_error("TreeMap negative capacity");
        if (n <= capacity())
            return;
        size_t old = values.size();
        values.resize(static_cast<size_t>(n) + 1);
        try {
            nodes.resize(static_cast<size_t>(n) + 1);
        } catch (...) {
            values.resize(old);
            throw;
        }
    }

    void clear() {
        this->resetValues(1, used + 1);
        root = freeHead = used = 0;
    }

    bool erase(const Key& key) {
        bool erased = false;
        root = eraseNode(root, key, erased);
        return erased;
    }

    int rankOf(Key key) const {
        u32 u = root;
        int result = 0;
        while (u) {
            if (compare(nodes[u].key, key)) {
                result += nodes[nodes[u].l].sz + 1;
                u = nodes[u].r;
            } else
                u = nodes[u].l;
        }
        return result;
    }
};

// Offline backend: only the registered key universe can be written to.
template<class Key, class Value, class Compare = std::less<Key>>
class TreeMapOff : public MapApi<Key, Value, TreeMapOff<Key, Value, Compare>> {
    friend class MapApi<Key, Value, TreeMapOff>;

    struct Cursor {
        u32 id = 0;
        u32 current() const { return id; }
    };

    Compare compare;
    std::vector<Key> keys;
    std::vector<Value> values;
    std::vector<u32> bit;
    std::vector<unsigned char> alive;
    u32 count = 0, bitStep = 0;

    const Key& keyFromIndex(u32 u) const { return keys[u - 1]; }

    size_t lowerIndex(const Key& key) const {
        return std::lower_bound(keys.begin(), keys.end(), key, compare) - keys.begin();
    }

    bool registered(size_t p, const Key& key) const {
        return p < keys.size() && !compare(key, keys[p]);
    }

    void addCount(u32 u, int delta) {
        for (u32 i = u; i < bit.size(); i += i & -i)
            bit[i] += delta;
    }

    u32 prefixCount(size_t p) const {
        u32 result = 0;
        for (; p; p -= p & -p)
            result += bit[p];
        return result;
    }

    u32 selectCount(u32 k) const {
        u32 u = 0;
        for (u32 step = bitStep; step; step >>= 1) {
            u32 next = u + step;
            if (next < bit.size() && bit[next] <= k) {
                u = next;
                k -= bit[next];
            }
        }
        return u + 1;
    }

    template<int Mode>
    std::pair<u32, bool> write(Key& key, Value* value) {
        size_t p = lowerIndex(key);
        if (!registered(p, key))
            throw std::out_of_range("TreeMapOff unregistered key");
        u32 u = static_cast<u32>(p + 1);
        if (alive[u]) {
            if constexpr (Mode == 1)
                values[u] = std::move(*value);
            return {u, false};
        }
        keys[p] = std::move(key); // An equivalent representative preserves the sorted universe.
        if constexpr (Mode == 2)
            values[u] = Value{};
        else
            values[u] = std::move(*value);
        alive[u] = 1;
        addCount(u, 1);
        ++count;
        return {u, true};
    }

    u32 findIndex(const Key& key) const {
        size_t p = lowerIndex(key);
        return registered(p, key) && alive[p + 1] ? p + 1 : 0;
    }

    u32 orderIndex(int k) const {
        return k >= 0 && static_cast<u32>(k) < count ? selectCount(k) : 0;
    }

    void beginCursor(Cursor& cursor) const {
        nextCursor(cursor);
    }

    void nextCursor(Cursor& cursor) const {
        for (size_t u = cursor.id + 1; u < alive.size(); ++u)
            if (alive[u]) {
                cursor.id = u;
                return;
            }
        cursor.id = 0;
    }

public:
    explicit TreeMapOff(std::vector<Key> candidates, Compare cmp = {})
        : compare(std::move(cmp)), keys(std::move(candidates)) {
        keys = std::move(keys) | seq::sorted(compare);
        keys.erase(
            std::unique(keys.begin(), keys.end(), [&](const Key& a, const Key& b) {
                return !compare(a, b);
            }),
            keys.end());
        values.resize(keys.size() + 1);
        bit.resize(keys.size() + 1);
        alive.resize(keys.size() + 1);
        if (!keys.empty())
            for (bitStep = 1; bitStep <= keys.size() / 2; bitStep <<= 1) {}
    }

    TreeMapOff(const TreeMapOff&) = default;

    TreeMapOff(TreeMapOff&& other) noexcept(std::is_nothrow_copy_constructible_v<Compare>)
        : compare(other.compare),
          keys(std::move(other.keys)),
          values(std::move(other.values)),
          bit(std::move(other.bit)),
          alive(std::move(other.alive)),
          count(std::exchange(other.count, 0)),
          bitStep(std::exchange(other.bitStep, 0)) {
        other.keys.clear();
        other.values.clear();
        other.bit.clear();
        other.alive.clear();
    }

    TreeMapOff& operator=(TreeMapOff other) noexcept(std::is_nothrow_swappable_v<Compare>) {
        using std::swap;
        swap(compare, other.compare);
        swap(count, other.count);
        swap(bitStep, other.bitStep);
        keys.swap(other.keys);
        values.swap(other.values);
        bit.swap(other.bit);
        alive.swap(other.alive);
        return *this;
    }

    int size() const { return static_cast<int>(count); }

    void clear() {
        this->resetValues(1, static_cast<u32>(values.size()));
        std::fill(bit.begin(), bit.end(), 0);
        std::fill(alive.begin(), alive.end(), 0);
        count = 0;
    }

    bool erase(const Key& key) {
        u32 u = findIndex(key);
        if (!u)
            return false;
        this->resetValues(u, u + 1);
        alive[u] = 0;
        addCount(u, -1);
        --count;
        return true;
    }

    int rankOf(Key key) const { return prefixCount(lowerIndex(key)); }
};}

using _treemap::TreeMap;
using _treemap::TreeMapOff;
