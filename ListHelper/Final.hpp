#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

// SNIPPET BEGIN
namespace seq {

template <class F>
struct Op {
    F f;

    template <class List>
    friend decltype(auto) operator|(List &&a, Op &op) {
        return std::invoke(op.f, std::forward<List>(a));
    }

    template <class List>
    friend decltype(auto) operator|(List &&a, Op &&op) {
        return std::invoke(op.f, std::forward<List>(a));
    }

    template <class List>
    friend decltype(auto) operator|(List &&a, const Op &op) {
        if constexpr (std::is_invocable_v<const F &, List &&>)
            return std::invoke(op.f, std::forward<List>(a));
        else {
            static_assert(std::is_copy_constructible_v<F>,
                          "a const mutable operation must be copyable");
            auto copy = op.f;
            return std::invoke(copy, std::forward<List>(a));
        }
    }
};
template <class F>
Op(F) -> Op<F>;

template <class T>
constexpr bool canMove =
    !std::is_lvalue_reference_v<T> && !std::is_const_v<std::remove_reference_t<T>> &&
    !std::is_same_v<std::decay_t<T>, std::string_view>;

template <class List>
auto materialize(List &&a) {
    if constexpr (std::is_same_v<std::decay_t<List>, std::string_view>)
        return std::string(a);
    else
        return std::decay_t<List>(std::forward<List>(a));
}

template <class List>
decltype(auto) listValue(const List &a, std::size_t i) {
    if constexpr (std::is_same_v<typename List::value_type, bool>)
        return bool(a[i]);
    else
        return (a[i]);
}

// Relative unsigned keys handle signed values without overflow.
template <int B, bool Descending = false, class List>
void radixSort(List &a, typename List::value_type minV, typename List::value_type maxV) {
    using T = typename List::value_type;
    static_assert(B > 0 && B <= 16 && std::is_integral_v<T> && !std::is_same_v<T, bool>);
    using U = std::make_unsigned_t<T>;
    auto key = [=](T x) { return U(Descending ? U(maxV) - U(x) : U(x) - U(minV)); };
    constexpr int mask = (1 << B) - 1;
    const std::size_t n = a.size();
    assert(!(maxV < minV));
    if (n < 2 || minV == maxV) return;
    std::vector<T> b(n);
    std::array<std::size_t, 1 << B> cnt{};
    bool buf = false;
    auto range = +U(U(maxV) - U(minV));
    for (int shift = 0; range; shift += B, range >>= B) {
        auto pass = [&](auto first, auto out) {
            cnt.fill(0);
            for (std::size_t j = 0; j < n; ++j)
                ++cnt[(key(first[j]) >> shift) & mask];
            if (cnt[(key(first[0]) >> shift) & mask] == n) return false;
            for (std::size_t j = 1; j < cnt.size(); ++j)
                cnt[j] += cnt[j - 1];
            for (std::size_t j = n; j-- > 0;)
                out[--cnt[(key(first[j]) >> shift) & mask]] = first[j];
            return true;
        };
        bool chg = buf ? pass(b.begin(), a.begin()) : pass(a.begin(), b.begin());
        buf ^= chg;
    }
    if (buf) std::move(b.begin(), b.end(), a.begin());
}

template <class Compare = std::less<>>
auto sorted(Compare cmp = {}) {
    return Op{[cmp = std::move(cmp)](auto &&a) mutable {
        auto result = materialize(std::forward<decltype(a)>(a));
        if (result.size() < 2) return result;
        if (cmp(result.back(), result.front())) {
            if (std::is_sorted(result.rbegin(), result.rend(), std::ref(cmp))) {
                std::reverse(result.begin(), result.end());
                return result;
            }
        } else if (std::is_sorted(result.begin(), result.end(), std::ref(cmp))) {
            return result;
        }
        using T = typename decltype(result)::value_type;
        constexpr bool ascending =
            std::is_same_v<Compare, std::less<T>> || std::is_same_v<Compare, std::less<>>;
        constexpr bool descending =
            std::is_same_v<Compare, std::greater<T>> || std::is_same_v<Compare, std::greater<>>;
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

template <class Equal = std::equal_to<>>
auto unique(Equal eq = {}) {
    return Op{[eq = std::move(eq)](auto &&a) mutable {
        auto result = materialize(std::forward<decltype(a)>(a));
        result.erase(std::unique(result.begin(), result.end(), std::ref(eq)), result.end());
        return result;
    }};
}

inline auto reverse() {
    return Op{[](auto &&a) {
        auto result = materialize(std::forward<decltype(a)>(a));
        std::reverse(result.begin(), result.end());
        return result;
    }};
}

inline auto slice(std::size_t first = 0,
                  std::size_t last = std::numeric_limits<std::size_t>::max()) {
    return Op{[first, last](auto &&a) {
        auto l = std::min(first, a.size()), r = std::max(l, std::min(last, a.size()));
        if constexpr (canMove<decltype(a)>) {
            a.erase(a.begin() + r, a.end());
            a.erase(a.begin(), a.begin() + l);
            return std::move(a);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(a)>, std::string_view>) {
            if (l == r) return std::string{};
            return std::string(a.begin() + l, a.begin() + r);
        } else {
            return std::decay_t<decltype(a)>(a.begin() + l, a.begin() + r, a.get_allocator());
        }
    }};
}

template <class F>
auto filter(F pred) {
    return Op{[pred = std::move(pred)](auto &&a) mutable {
        if constexpr (canMove<decltype(a)>) {
            a.erase(std::remove_if(a.begin(), a.end(), [&](const auto &x) { return !pred(x); }),
                    a.end());
            return std::move(a);
        } else {
            auto result = [&] {
                if constexpr (std::is_same_v<std::decay_t<decltype(a)>, std::string_view>)
                    return std::string{};
                else
                    return std::decay_t<decltype(a)>(a.get_allocator());
            }();
            result.reserve(a.size());
            std::copy_if(a.begin(), a.end(), std::back_inserter(result), std::ref(pred));
            return result;
        }
    }};
}

template <class F>
auto map(F f) {
    return Op{[f = std::move(f)](const auto &a) mutable {
        std::vector<std::decay_t<decltype(f(listValue(a, 0)))>> result;
        result.reserve(a.size());
        for (std::size_t i = 0; i < a.size(); ++i)
            result.push_back(f(listValue(a, i)));
        return result;
    }};
}

inline auto enumerate() {
    return Op{[](const auto &a) {
        std::vector<std::pair<typename std::decay_t<decltype(a)>::value_type, std::size_t>> result;
        result.reserve(a.size());
        for (std::size_t i = 0; i < a.size(); ++i)
            result.emplace_back(a[i], i);
        return result;
    }};
}

template <class Input, class T, class Allocator>
void read(Input &in, std::vector<T, Allocator> &a) {
    for (auto &x : a)
        if (!(in >> x)) break;
}

template <class Input, class Allocator>
void read(Input &in, std::vector<bool, Allocator> &a) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        bool x;
        if (!(in >> x)) break;
        a[i] = x;
    }
}

template <class Input, class Traits, class Allocator>
void read(Input &in, std::basic_string<char, Traits, Allocator> &a) {
    in >> a;
}

template <class Input = std::istream>
auto readFrom(Input &in = std::cin) {
    return Op{[&in](auto &&a) {
        auto result = materialize(std::forward<decltype(a)>(a));
        read(in, result);
        return result;
    }};
}

template <class Input = std::istream>
auto cin(Input &in = std::cin) {
    return readFrom(in);
}

// Terminal operations: read the container without copying it.
template <class F>
auto count(F pred) {
    return Op{[pred = std::move(pred)](const auto &a) mutable -> std::size_t {
        return std::count_if(a.begin(), a.end(), std::ref(pred));
    }};
}

template <class F>
auto first(F pred) {
    return Op{[pred = std::move(pred)](auto &&a) mutable {
        using Result = std::optional<typename std::decay_t<decltype(a)>::value_type>;
        const auto &ro = a;
        auto it = std::find_if(ro.begin(), ro.end(), std::ref(pred));
        if (it == ro.end()) return Result{};
        if constexpr (canMove<decltype(a)>) {
            auto it2 = a.begin();
            std::advance(it2, std::distance(ro.begin(), it));
            return Result(std::in_place, std::move(*it2));
        } else
            return Result(std::in_place, *it);
    }};
}

template <class T = std::monostate, class F = std::plus<>>
auto accumulate(T initial = {}, F f = {}) {
    return Op{[initial = std::move(initial), f = std::move(f)](const auto &a) mutable {
        auto result = [&] {
            if constexpr (std::is_same_v<T, std::monostate>)
                return typename std::decay_t<decltype(a)>::value_type{};
            else if constexpr (std::is_copy_constructible_v<T>)
                return T(initial);
            else
                return std::move(initial);
        }();
        for (const auto &x : a)
            result = f(std::move(result), x);
        return result;
    }};
}

template <class Output, class T, class Allocator>
void write(Output &out, const std::vector<T, Allocator> &a, const std::string &separator) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (i) out << separator;
        out << a[i];
    }
}

template <class Output, class Traits, class Allocator>
void write(Output &out, const std::basic_string<char, Traits, Allocator> &a, const std::string &) {
    out << a;
}

template <class Output>
void write(Output &out, std::string_view text, const std::string &) {
    out << text;
}

template <class Output = std::ostream>
auto writeTo(Output &out = std::cout, std::string separator = " ", std::string ending = "\n") {
    return Op{[&out, separator = std::move(separator), ending = std::move(ending)](const auto &a) {
        write(out, a, separator);
        out << ending;
    }};
}

template <class Output = std::ostream>
auto cout(Output &out = std::cout, std::string separator = " ", std::string ending = "\n") {
    return writeTo(out, std::move(separator), std::move(ending));
}

// Borrow const lvalues; consume rvalues. Materialize temporary reference results.
template <class F, class... Args>
struct MemberCall {
    F function;
    std::tuple<Args...> args;

    explicit MemberCall(F function, Args &&...args)
        : function(std::move(function)), args(std::forward<Args>(args)...) {}
};
template <class F, class... Args>
MemberCall(F, Args &&...) -> MemberCall<F, Args...>;

template <class T, class Op>
decltype(auto) applyCall(T &&object, Op &&op) {
    using ConstObject = std::add_const_t<std::remove_reference_t<T>>;
    using Receiver = std::conditional_t<std::is_lvalue_reference_v<T>, ConstObject &, T &&>;
    auto run = [&]() -> decltype(auto) {
        return std::apply(
            [&](auto &&...args) -> decltype(auto) {
                return std::invoke(std::forward<Op>(op).function, static_cast<Receiver>(object),
                                   std::forward<decltype(args)>(args)...);
            },
            std::forward<Op>(op).args);
    };
    if constexpr (std::is_reference_v<decltype(run())> && !std::is_lvalue_reference_v<T>) {
        using Value = std::decay_t<decltype(run())>;
        return Value(std::move(run()));
    } else if constexpr (!std::is_void_v<decltype(run())>)
        return run();
    else {
        run();
        if constexpr (std::is_lvalue_reference_v<T>)
            return static_cast<Receiver>(object);
        else
            return std::decay_t<T>(std::forward<T>(object));
    }
}

#define callOperator(ref) \
    template <class T, class F, class... Args> \
    decltype(auto) operator|(T &&object, MemberCall<F, Args...> ref op) { \
        return applyCall(std::forward<T>(object), std::forward<decltype(op)>(op)); \
    }
callOperator(&)
callOperator(const &)
callOperator(&&)
#undef callOperator

} // namespace seq

using seq::enumerate, seq::count, seq::first, seq::accumulate, seq::readFrom, seq::writeTo;
using seq::sorted, seq::unique, seq::reverse, seq::slice, seq::filter, seq::map;

#define call(name, ...) \
    ::seq::MemberCall( \
        [](auto &&self, auto &&...args) -> decltype(auto) { \
            return std::forward<decltype(self)>(self).name(std::forward<decltype(args)>(args)...); \
        }, \
        ##__VA_ARGS__)
