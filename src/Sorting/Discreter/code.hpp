#pragma once
#include "Include.hpp"
// Sorted basis: borrow lvalues and own rvalues.
template<class List>
auto discreteFrom(List&& b) {
    return seq::Op{[t = std::tuple<List>(std::forward<List>(b))](const auto& a) {
        const auto& b = std::get<0>(t);
        std::vector<int> res(a.size());
        for (std::size_t i = 0; i < a.size(); ++i)
            res[i] = std::lower_bound(b.begin(), b.end(), a[i]) - b.begin();
        return res;
    }};
}
