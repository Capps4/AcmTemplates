#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T, class Cmp = std::greater_equal<T>>
class MonotonyStack {
    std::vector<std::pair<T, int>> st{};
    Cmp cmp;

public:
    MonotonyStack(const T& val, int id, Cmp cmp = {}) : cmp(std::move(cmp)) {
        st.emplace_back(val, id);
    }

    // The initial sentinel must never satisfy cmp(sentinel, val).
    int push(const T& val, int id) {
        while (!st.empty() and cmp(st.back().first, val)) {
            st.pop_back();
        }
        assert(!st.empty());
        int ans = st.back().second;
        st.emplace_back(val, id);
        return ans;
    }
};
