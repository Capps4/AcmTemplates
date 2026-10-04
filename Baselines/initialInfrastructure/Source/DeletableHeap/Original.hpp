#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T, class Cmp = std::less<T>>
class DeletableHeap {
    std::priority_queue<T, std::vector<T>, Cmp> items{};
    std::priority_queue<T, std::vector<T>, Cmp> trash{};

    void sync() {
        while (!trash.empty() and items.top() == trash.top()) {
            items.pop();
            trash.pop();
        }
    }
public:
    int size() {
        assert(items.size() >= trash.size());
        return items.size() - trash.size();
    }

    void push(T x) {
        items.push(x);
    }

    void erase(T x) {
        trash.push(x);
    }

    T top() {
        sync();
        return items.top();
    }

    void pop() {
        sync();
        items.pop();
    }
};

