#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T = long long>
class SlopeTrick {
    std::priority_queue<T> l{};
    std::priority_queue<T, std::vector<T>, std::greater<T>> r{};
    T dl{}, dr{}, mn{};

public:
    void addConstant(T x) {
        mn += x;
    }

    // f(x) += max(a - x, 0).
    void addLeft(T a) {
        r.push(a - dr);
        T x = r.top() + dr;
        r.pop();
        mn += a - x;
        l.push(x - dl);
    }

    // f(x) += max(x - a, 0).
    void addRight(T a) {
        l.push(a - dl);
        T x = l.top() + dl;
        l.pop();
        mn += x - a;
        r.push(x - dr);
    }

    void addAbs(T a) {
        addLeft(a);
        addRight(a);
    }

    // f(x) <- min f(y), x - b <= y <= x - a.
    void shift(T a, T b) {
        assert(a <= b);
        dl += a;
        dr += b;
    }

    void shift(T a) {
        shift(a, a);
    }

    // f(x) <- min_{y <= x} f(y).
    void prefixMin() {
        r = {};
    }

    // f(x) <- min_{y >= x} f(y).
    void suffixMin() {
        l = {};
    }

    // One minimizer and the minimum value; the zero function chooses x = 0.
    std::pair<T, T> minimum() const {
        T x = not l.empty() ? l.top() + dl : not r.empty() ? r.top() + dr : T{};
        return {x, mn};
    }
};
