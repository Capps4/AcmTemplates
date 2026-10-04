#pragma once
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
namespace seq {

template<class F>
struct Op {
    F f;
    template<class List>
    friend decltype(auto) operator|(List&& a, Op op) {
        return op.f(std::forward<List>(a));
    }
};
template<class F> Op(F) -> Op<F>;

template<class T>
constexpr bool canMove = !std::is_lvalue_reference_v<T> &&
                        !std::is_const_v<std::remove_reference_t<T>>;

template<int B, bool Descending = false, class List>
void radixSort(List& a, typename List::value_type minV,
               typename List::value_type maxV) {
    using T = typename List::value_type;
    using U = std::make_unsigned_t<T>;
    auto key = [=](T x) {
        return U(Descending ? U(maxV) - U(x) : U(x) - U(minV));
    };
    constexpr int mask = (1 << B) - 1;
    const std::size_t n = a.size();
    std::vector<T> b(n);
    std::array<std::size_t, 1 << B> cnt{};
    bool inBuffer = false;
    auto range = +U(U(maxV) - U(minV));
    for (int shift = 0; range; shift += B, range >>= B) {
        auto pass = [&](auto first, auto output) {
            cnt.fill(0);
            for (std::size_t j = 0; j < n; ++j)
                ++cnt[(key(first[j]) >> shift) & mask];
            if (cnt[(key(first[0]) >> shift) & mask] == n)
                return false;
            for (std::size_t j = 1; j < cnt.size(); ++j)
                cnt[j] += cnt[j - 1];
            for (std::size_t j = n; j-- > 0;)
                output[--cnt[(key(first[j]) >> shift) & mask]] = first[j];
            return true;
        };
        bool changed = inBuffer ? pass(b.begin(), a.begin())
                                : pass(a.begin(), b.begin());
        inBuffer ^= changed;
    }
    if (inBuffer)
        std::move(b.begin(), b.end(), a.begin());
}

template<class Compare = std::less<>>
auto sorted(Compare cmp = {}) {
    return Op{[cmp = std::move(cmp)](auto&& a) mutable {
        auto result = std::forward<decltype(a)>(a);
        if (result.size() < 2)
            return result;
        if (cmp(result.back(), result.front())) {
            if (std::is_sorted(result.rbegin(), result.rend(), std::ref(cmp))) {
                std::reverse(result.begin(), result.end());
                return result;
            }
        } else if (std::is_sorted(result.begin(), result.end(), std::ref(cmp))) {
            return result;
        }
        using T = typename decltype(result)::value_type;
        constexpr bool ascending = std::is_same_v<Compare, std::less<T>> || std::is_same_v<Compare, std::less<>>;
        constexpr bool descending = std::is_same_v<Compare, std::greater<T>> || std::is_same_v<Compare, std::greater<>>;
        if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool> &&
                      (ascending || descending)) {
            if (result.size() >= 256) {
                auto [lo, hi] = std::minmax_element(result.begin(), result.end());
                using U = std::make_unsigned_t<T>;
                int bits = 0;
                for (auto range = +U(U(*hi) - U(*lo)); range; range >>= 1)
                    ++bits;
                int passes = (bits + 7) / 8;
                if (result.size() >= 256u * passes) {
                    if (result.size() >= 4096 && (bits + 10) / 11 < passes)
                        radixSort<11, descending>(result, *lo, *hi);
                    else
                        radixSort<8, descending>(result, *lo, *hi);
                    return result;
                }
            }
        }
        if constexpr (ascending || descending)
            std::sort(result.begin(), result.end(), cmp);
        else
            std::sort(result.begin(), result.end(), std::ref(cmp));
        return result;
    }};
}
template<class Equal = std::equal_to<>>
auto unique(Equal eq = {}) {
    return Op{[eq = std::move(eq)](auto&& a) mutable {
        auto result = std::forward<decltype(a)>(a);
        result.erase(std::unique(result.begin(), result.end(), std::ref(eq)), result.end());
        return result;
    }};
}
inline auto reverse() {
    return Op{[](auto&& a) {
        auto result = std::forward<decltype(a)>(a);
        std::reverse(result.begin(), result.end());
        return result;
    }};
}
inline auto slice(std::size_t first = 0,
                  std::size_t last = std::numeric_limits<std::size_t>::max()) {
    return Op{[first, last](auto&& a) {
        auto l = std::min(first, a.size()), r = std::max(l, std::min(last, a.size()));
        if constexpr (canMove<decltype(a)>) {
            a.erase(a.begin() + r, a.end());
            a.erase(a.begin(), a.begin() + l);
            return std::move(a);
        } else {
            return std::decay_t<decltype(a)>(a.begin() + l, a.begin() + r, a.get_allocator());
        }
    }};
}
template<class F>
auto filter(F pred) {
    return Op{[pred = std::move(pred)](auto&& a) mutable {
        if constexpr (canMove<decltype(a)>) {
            a.erase(std::remove_if(a.begin(), a.end(), [&](const auto& x) { return !pred(x); }), a.end());
            return std::move(a);
        } else {
            std::decay_t<decltype(a)> result(a.get_allocator());
            result.reserve(a.size());
            std::copy_if(a.begin(), a.end(), std::back_inserter(result), std::ref(pred));
            return result;
        }
    }};
}
template<class F>
auto map(F f) {
    return Op{[f = std::move(f)](const auto& a) mutable {
        std::vector<std::decay_t<decltype(f(a[0]))>> result;
        result.reserve(a.size());
        for (std::size_t i = 0; i < a.size(); ++i) result.push_back(f(a[i]));
        return result;
    }};
}
inline auto enumerate() {
    return Op{[](const auto& a) {
        std::vector<std::pair<typename std::decay_t<decltype(a)>::value_type, std::size_t>> result;
        result.reserve(a.size());
        for (std::size_t i = 0; i < a.size(); ++i) result.emplace_back(a[i], i);
        return result;
    }};
}

template<class T, class Allocator>
void read(std::istream& in, std::vector<T, Allocator>& a) {
    for (auto& x : a) if (!(in >> x)) break;
}
template<class Allocator>
void read(std::istream& in, std::vector<bool, Allocator>& a) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        bool x;
        if (!(in >> x)) break;
        a[i] = x;
    }
}
template<class Traits, class Allocator>
void read(std::istream& in, std::basic_string<char, Traits, Allocator>& a) { in >> a; }
inline auto cin(std::istream& in = std::cin) {
    return Op{[&in](auto&& a) {
        auto result = std::forward<decltype(a)>(a);
        read(in, result);
        return result;
    }};
}

// Terminal operations: read the container without copying it.
template<class F>
auto count(F pred) {
    return Op{[pred = std::move(pred)](const auto& a) mutable -> std::size_t {
        return std::count_if(a.begin(), a.end(), std::ref(pred));
    }};
}
template<class F>
auto first(F pred) {
    return Op{[pred = std::move(pred)](const auto& a) mutable {
        using Result = std::optional<typename std::decay_t<decltype(a)>::value_type>;
        auto it = std::find_if(a.begin(), a.end(), std::ref(pred));
        return it == a.end() ? Result{} : Result(std::in_place, *it);
    }};
}
template<class T = std::monostate, class F = std::plus<>>
auto accumulate(T initial = {}, F f = {}) {
    return Op{[initial = std::move(initial), f = std::move(f)](const auto& a) mutable {
        auto result = [&] {
            if constexpr (std::is_same_v<T, std::monostate>) return typename std::decay_t<decltype(a)>::value_type{};
            else return std::move(initial);
        }();
        for (const auto& x : a) result = f(std::move(result), x);
        return result;
    }};
}

template<class T, class Allocator>
void write(std::ostream& out, const std::vector<T, Allocator>& a, const std::string& separator) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (i) out << separator;
        out << a[i];
    }
}
template<class Traits, class Allocator>
void write(std::ostream& out, const std::basic_string<char, Traits, Allocator>& a,
           const std::string&) { out << a; }
inline auto cout(std::ostream& out = std::cout, std::string separator = " ", std::string ending = "\n") {
    return Op{[&out, separator = std::move(separator), ending = std::move(ending)](const auto& a) {
        write(out, a, separator);
        out << ending;
    }};
}

// General member calls: preserve the existing forwarding and return semantics.
template<class F, class... Args>
struct memberCall {
    F function;
    std::tuple<Args...> args;
    explicit memberCall(F function, Args&&... args)
        : function(std::move(function)), args(std::forward<Args>(args)...) {}
};
template<class F, class... Args>
memberCall(F, Args&&...) -> memberCall<F, Args...>;

template<class T, class Op>
decltype(auto) applyCall(T&& object, Op&& op) {
    using constObject = std::add_const_t<std::remove_reference_t<T>>;
    using receiver = std::conditional_t<std::is_lvalue_reference_v<T>, constObject&, constObject&&>;
    auto run = [&]() -> decltype(auto) {
        return std::apply([&](auto&&... args) -> decltype(auto) {
            return std::forward<Op>(op).function(static_cast<receiver>(object),
                                                std::forward<decltype(args)>(args)...);
        }, std::forward<Op>(op).args);
    };
    if constexpr (!std::is_void_v<decltype(run())>) return run();
    else {
        run();
        if constexpr (std::is_lvalue_reference_v<T>) return static_cast<receiver>(object);
        else return std::decay_t<T>(std::forward<T>(object));
    }
}
#define callOperator(ref) \
    template<class T, class F, class... Args> \
    decltype(auto) operator|(T&& object, memberCall<F, Args...> ref op) { \
        return applyCall(std::forward<T>(object), std::forward<decltype(op)>(op)); \
    }
callOperator(&)
callOperator(const&)
callOperator(&&)
#undef callOperator

}

using seq::sorted, seq::unique, seq::reverse, seq::slice, seq::filter, seq::map;
using seq::enumerate, seq::count, seq::first, seq::accumulate, seq::cin, seq::cout;

#define call(name, ...) \
    ::seq::memberCall([](auto&& self, auto&&... args) -> decltype(auto) { \
        return std::forward<decltype(self)>(self).name( \
            std::forward<decltype(args)>(args)...); \
    }, ##__VA_ARGS__)
