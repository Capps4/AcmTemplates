#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T, class F = std::plus<T>>
class SWAG {
    struct Node {
        T val, sum;
    };
    F f;
    std::vector<Node> a{}, b{};

public:
    explicit SWAG(F f = {}) : f(std::move(f)) {}

    bool empty() const {
        return a.empty() and b.empty();
    }

    std::size_t size() const {
        return a.size() + b.size();
    }

    void push(T val) {
        T sum = a.empty() ? val : f(a.back().sum, val);
        a.push_back({std::move(val), std::move(sum)});
    }

    void pop() {
        assert(not empty());
        if (b.empty()) {
            while (not a.empty()) {
                T val = std::move(a.back().val);
                T sum = b.empty() ? val : f(val, b.back().sum);
                b.push_back({std::move(val), std::move(sum)});
                a.pop_back();
            }
        }
        b.pop_back();
    }

    T query() const {
        assert(not empty());
        if (a.empty()) {
            return b.back().sum;
        }
        if (b.empty()) {
            return a.back().sum;
        }
        return f(b.back().sum, a.back().sum);
    }
};
