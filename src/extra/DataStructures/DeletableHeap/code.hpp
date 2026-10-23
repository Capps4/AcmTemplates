#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
// erase must remove a present value; comparator equivalence must agree with ==.
template <class T, class Cmp = std::less<T>>
class DeletableHeap {
    std::priority_queue<T, std::vector<T>, Cmp> heap, del;

    void sync() {
        while (!del.empty() and heap.top() == del.top()) {
            heap.pop();
            del.pop();
        }
    }

public:
    explicit DeletableHeap(Cmp cmp = {}) : heap(cmp), del(cmp) {}

    explicit DeletableHeap(std::vector<T> a, Cmp cmp = {}) : heap(cmp, std::move(a)), del(cmp) {}

    std::size_t size() const {
        assert(heap.size() >= del.size());
        return heap.size() - del.size();
    }

    bool empty() const {
        return size() == 0;
    }

    void push(T v) {
        heap.push(std::move(v));
    }

    template <class... Args>
    void emplace(Args &&...args) {
        heap.emplace(std::forward<Args>(args)...);
    }

    void erase(T v) {
        del.push(std::move(v));
    }

    const T &top() {
        sync();
        assert(!heap.empty());
        return heap.top();
    }

    void pop() {
        sync();
        assert(!heap.empty());
        heap.pop();
    }
};
