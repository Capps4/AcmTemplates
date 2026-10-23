#pragma once
#include "../../../../Headers/Headers.hpp"

template <class T, class Cmp = std::less<T>>
struct DecareTree {
    Cmp cmp;
    std::vector<std::pair<int, int>> t;
    int root = -1;

    explicit DecareTree(const std::vector<T>& a, Cmp cmp = {})
        : cmp(std::move(cmp)), t(a.size(), {-1, -1}) {
        std::vector<int> st{};
        for (int i = 0; i < int(a.size()); ++i) {
            int p = -1;
            while (!st.empty() and !this->cmp(a[st.back()], a[i])) {
                p = st.back();
                st.pop_back();
            }
            t[i].first = p;
            if (!st.empty()) {
                t[st.back()].second = i;
            }
            st.push_back(i);
        }
        if (!st.empty()) {
            root = st.front();
        }
    }
};
