#pragma once
#include <algorithm>
#include <functional>
#include <tuple>
#include <utility>
#include <vector>
#include "../ListHelper/Final.hpp"

// SNIPPET BEGIN
// The small sorted basis required by Original PersistentTree.
template <class T>
class Discreter {
    std::vector<T> values;

public:
    explicit Discreter(std::vector<T> a = {}) : values(std::move(a)) {
        values = std::move(values) | seq::sorted();
        values.erase(std::unique(values.begin(), values.end()), values.end());
    }

    int size() const { return values.size(); }

    int rankOf(const T &v) const {
        return std::lower_bound(values.begin(), values.end(), v) - values.begin();
    }

    T at(int rank) const { return values[rank]; }

    auto begin() const { return values.begin(); }

    auto end() const { return values.end(); }
};

// Sorted lvalues are borrowed; rvalues are owned by the operation.
template <class List, class Compare = std::less<>>
auto discreteFrom(List &&values, Compare cmp = {}) {
    return seq::Op{[values = std::tuple<List>(std::forward<List>(values)),
                    cmp = std::move(cmp)](const auto &a) mutable {
        const auto &basis = std::get<0>(values);
        std::vector<int> result(a.size());
        for (std::size_t i = 0; i < a.size(); ++i)
            result[i] =
                std::lower_bound(basis.begin(), basis.end(), a[i], std::ref(cmp)) - basis.begin();
        return result;
    }};
}
