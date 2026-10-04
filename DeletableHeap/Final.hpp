#pragma once
#include <cassert>
#include <functional>
#include <queue>
#include <utility>
#include <vector>

// SNIPPET BEGIN
// erase must remove a present value; comparator equivalence must agree with ==.
template <class T, class Cmp = std::less<T>>
class DeletableHeap {
    std::priority_queue<T, std::vector<T>, Cmp> items, trash;

    void sync() {
        while (!trash.empty() && items.top() == trash.top()) {
            items.pop();
            trash.pop();
        }
    }

public:
    explicit DeletableHeap(Cmp cmp = {}) : items(cmp), trash(cmp) {}

    explicit DeletableHeap(std::vector<T> values, Cmp cmp = {})
        : items(cmp, std::move(values)), trash(cmp) {}

    std::size_t size() const {
        assert(items.size() >= trash.size());
        return items.size() - trash.size();
    }

    bool empty() const { return size() == 0; }

    void push(T value) { items.push(std::move(value)); }

    template <class... Args>
    void emplace(Args &&...args) {
        items.emplace(std::forward<Args>(args)...);
    }

    void erase(T value) { trash.push(std::move(value)); }

    const T &top() {
        sync();
        assert(!items.empty());
        return items.top();
    }

    void pop() {
        sync();
        assert(!items.empty());
        items.pop();
    }
};
