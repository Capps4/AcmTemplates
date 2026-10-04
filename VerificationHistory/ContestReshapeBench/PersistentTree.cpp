#include <bits/stdc++.h>
#include "/Users/bytedance/acm_compete/cpp17/BenchmarkSupport.hpp"
#include "/Users/bytedance/acm_compete/cpp17/Discreter/Final.hpp"
namespace Legacy {
// The original snippet refers to an absent Discreter; provide the same final dependency to both implementations.
template<class T>using Discreter=::Discreter<T>;
#include "/Users/bytedance/acm_compete/cpp17/PersistentTree/Original.hpp"
}
#include "/Users/bytedance/acm_compete/cpp17/PersistentTree/Final.hpp"
namespace Before {
namespace seq {

template<class F>
struct Op {
    F f;
    template<class List>
    friend decltype(auto) operator|(List&& a, Op& op) { return std::invoke(op.f, std::forward<List>(a)); }
    template<class List>
    friend decltype(auto) operator|(List&& a, Op&& op) { return std::invoke(op.f, std::forward<List>(a)); }
    template<class List>
    friend decltype(auto) operator|(List&& a, const Op& op) {
        if constexpr (std::is_invocable_v<const F&, List&&>) return std::invoke(op.f, std::forward<List>(a));
        else {
            static_assert(std::is_copy_constructible_v<F>, "a const mutable operation must be copyable");
            auto copy = op.f;
            return std::invoke(copy, std::forward<List>(a));
        }
    }
};
template<class F> Op(F) -> Op<F>;

template<class T>
constexpr bool canMove = !std::is_lvalue_reference_v<T> &&
                        !std::is_const_v<std::remove_reference_t<T>> &&
                        !std::is_same_v<std::decay_t<T>, std::string_view>;

template<class List>
auto materialize(List&& a) {
    if constexpr (std::is_same_v<std::decay_t<List>, std::string_view>) return std::string(a);
    else return std::decay_t<List>(std::forward<List>(a));
}
template<class List>
decltype(auto) listValue(const List& a, std::size_t i) {
    if constexpr (std::is_same_v<typename List::value_type, bool>) return bool(a[i]);
    else return (a[i]);
}

// Relative unsigned keys handle signed values without overflow.
template<int B, bool Descending = false, class List>
void radixSort(List& a, typename List::value_type minV,
               typename List::value_type maxV) {
    using T = typename List::value_type;
    static_assert(B > 0 && B <= 16 && std::is_integral_v<T> && !std::is_same_v<T, bool>);
    using U = std::make_unsigned_t<T>;
    auto key = [=](T x) {
        return U(Descending ? U(maxV) - U(x) : U(x) - U(minV));
    };
    constexpr int mask = (1 << B) - 1;
    const std::size_t n = a.size();
    assert(!(maxV < minV));
    if (n < 2 || minV == maxV) return;
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
        auto result = materialize(std::forward<decltype(a)>(a));
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
        auto result = materialize(std::forward<decltype(a)>(a));
        result.erase(std::unique(result.begin(), result.end(), std::ref(eq)), result.end());
        return result;
    }};
}
inline auto reverse() {
    return Op{[](auto&& a) {
        auto result = materialize(std::forward<decltype(a)>(a));
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
        } else if constexpr (std::is_same_v<std::decay_t<decltype(a)>, std::string_view>) {
            if (l == r) return std::string{};
            return std::string(a.begin() + l, a.begin() + r);
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
            auto result = [&] {
                if constexpr (std::is_same_v<std::decay_t<decltype(a)>, std::string_view>) return std::string{};
                else return std::decay_t<decltype(a)>(a.get_allocator());
            }();
            result.reserve(a.size());
            std::copy_if(a.begin(), a.end(), std::back_inserter(result), std::ref(pred));
            return result;
        }
    }};
}
template<class F>
auto map(F f) {
    return Op{[f = std::move(f)](const auto& a) mutable {
        std::vector<std::decay_t<decltype(f(listValue(a, 0)))>> result;
        result.reserve(a.size());
        for (std::size_t i = 0; i < a.size(); ++i) result.push_back(f(listValue(a, i)));
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

template<class Input, class T, class Allocator>
void read(Input& in, std::vector<T, Allocator>& a) {
    for (auto& x : a) if (!(in >> x)) break;
}
template<class Input, class Allocator>
void read(Input& in, std::vector<bool, Allocator>& a) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        bool x;
        if (!(in >> x)) break;
        a[i] = x;
    }
}
template<class Input, class Traits, class Allocator>
void read(Input& in, std::basic_string<char, Traits, Allocator>& a) { in >> a; }
template<class Input = std::istream>
auto readFrom(Input& in = std::cin) {
    return Op{[&in](auto&& a) {
        auto result = materialize(std::forward<decltype(a)>(a));
        read(in, result);
        return result;
    }};
}

template<class Input = std::istream>
auto cin(Input& in = std::cin) { return readFrom(in); }

// Terminal operations: read the container without copying it.
template<class F>
auto count(F pred) {
    return Op{[pred = std::move(pred)](const auto& a) mutable -> std::size_t {
        return std::count_if(a.begin(), a.end(), std::ref(pred));
    }};
}
template<class F>
auto first(F pred) {
    return Op{[pred = std::move(pred)](auto&& a) mutable {
        using Result = std::optional<typename std::decay_t<decltype(a)>::value_type>;
        const auto& readOnly = a;
        auto it = std::find_if(readOnly.begin(), readOnly.end(), std::ref(pred));
        if (it == readOnly.end()) return Result{};
        if constexpr (canMove<decltype(a)>) {
            auto writable = a.begin();
            std::advance(writable, std::distance(readOnly.begin(), it));
            return Result(std::in_place, std::move(*writable));
        } else return Result(std::in_place, *it);
    }};
}

template<class T = std::monostate, class F = std::plus<>>
auto accumulate(T initial = {}, F f = {}) {
    return Op{[initial = std::move(initial), f = std::move(f)](const auto& a) mutable {
        auto result = [&] {
            if constexpr (std::is_same_v<T, std::monostate>) return typename std::decay_t<decltype(a)>::value_type{};
            else if constexpr (std::is_copy_constructible_v<T>) return T(initial);
            else return std::move(initial);
        }();
        for (const auto& x : a) result = f(std::move(result), x);
        return result;
    }};
}

template<class Output, class T, class Allocator>
void write(Output& out, const std::vector<T, Allocator>& a, const std::string& separator) {
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (i) out << separator;
        out << a[i];
    }
}
template<class Output, class Traits, class Allocator>
void write(Output& out, const std::basic_string<char, Traits, Allocator>& a,
           const std::string&) { out << a; }
template<class Output>
void write(Output& out, std::string_view text, const std::string&) { out << text; }
template<class Output = std::ostream>
auto writeTo(Output& out = std::cout, std::string separator = " ", std::string ending = "\n") {
    return Op{[&out, separator = std::move(separator), ending = std::move(ending)](const auto& a) {
        write(out, a, separator);
        out << ending;
    }};
}

template<class Output = std::ostream>
auto cout(Output& out = std::cout, std::string separator = " ", std::string ending = "\n") {
    return writeTo(out, std::move(separator), std::move(ending));
}

// Borrow const lvalues; consume rvalues. Materialize temporary reference results.
template<class F, class... Args>
struct MemberCall {
    F function;
    std::tuple<Args...> args;
    explicit MemberCall(F function, Args&&... args)
        : function(std::move(function)), args(std::forward<Args>(args)...) {}
};
template<class F, class... Args>
MemberCall(F, Args&&...) -> MemberCall<F, Args...>;

template<class T, class Op>
decltype(auto) applyCall(T&& object, Op&& op) {
    using ConstObject = std::add_const_t<std::remove_reference_t<T>>;
    using Receiver = std::conditional_t<std::is_lvalue_reference_v<T>, ConstObject&, T&&>;
    auto run = [&]() -> decltype(auto) {
        return std::apply([&](auto&&... args) -> decltype(auto) {
            return std::invoke(std::forward<Op>(op).function, static_cast<Receiver>(object),
                               std::forward<decltype(args)>(args)...);
        }, std::forward<Op>(op).args);
    };
    if constexpr (std::is_reference_v<decltype(run())> && !std::is_lvalue_reference_v<T>) {
        using Value = std::decay_t<decltype(run())>;
        return Value(std::move(run()));
    } else if constexpr (!std::is_void_v<decltype(run())>) return run();
    else {
        run();
        if constexpr (std::is_lvalue_reference_v<T>) return static_cast<Receiver>(object);
        else return std::decay_t<T>(std::forward<T>(object));
    }
}
#define callOperator(ref) \
    template<class T, class F, class... Args> \
    decltype(auto) operator|(T&& object, MemberCall<F, Args...> ref op) { \
        return applyCall(std::forward<T>(object), std::forward<decltype(op)>(op)); \
    }
callOperator(&)
callOperator(const&)
callOperator(&&)
#undef callOperator

}

using seq::sorted, seq::unique, seq::reverse, seq::slice, seq::filter, seq::map;
using seq::enumerate, seq::count, seq::first, seq::accumulate, seq::readFrom, seq::writeTo;

#define call(name, ...) \
    ::seq::MemberCall([](auto&& self, auto&&... args) -> decltype(auto) { \
        return std::forward<decltype(self)>(self).name( \
            std::forward<decltype(args)>(args)...); \
    }, ##__VA_ARGS__)

template<class T, class Compare = std::less<>>
class Discreter {
    std::vector<T> values;
    Compare cmp;
    void build() {
        assert(values.size() < std::size_t(std::numeric_limits<int>::max()));
        std::sort(values.begin(), values.end(), std::ref(cmp));
        values.erase(std::unique(values.begin(), values.end(), [&](const T& a, const T& b) {
            return !cmp(a, b) && !cmp(b, a);
        }), values.end());
    }
public:
    static_assert(std::is_invocable_r_v<bool, const Compare&, const T&, const T&>);
    using value_type = T;
    using ConstReference = std::conditional_t<std::is_same_v<T, bool>, bool, const T&>;
    explicit Discreter(std::vector<T> input = {}, Compare compare = {})
        : values(std::move(input)), cmp(std::move(compare)) { build(); }
    Discreter(std::initializer_list<T> input, Compare compare = {})
        : Discreter(std::vector<T>(input), std::move(compare)) {}
    template<class Range>
    explicit Discreter(const Range& input, Compare compare = {})
        : Discreter(std::vector<T>(input.begin(), input.end()), std::move(compare)) {}
    int size() const { return int(values.size()); }
    int rankOf(const T& value) const {
        return int(std::lower_bound(values.begin(), values.end(), value, std::cref(cmp)) - values.begin());
    }
    int upperRankOf(const T& value) const {
        return int(std::upper_bound(values.begin(), values.end(), value, std::cref(cmp)) - values.begin());
    }
    bool contains(const T& value) const {
        int rank = rankOf(value);
        return rank < size() && !cmp(value, values[rank]);
    }
    ConstReference at(int rank) const & {
        assert(0 <= rank && rank < size());
        if constexpr (std::is_same_v<T, bool>) return bool(values[rank]);
        else return values[rank];
    }
    T at(int rank) const && { return static_cast<const Discreter&>(*this).at(rank); }
    auto begin() const { return values.begin(); }
    auto end() const { return values.end(); }
    const Compare& comparator() const { return cmp; }
};
template<class T, class Compare = std::less<>>
Discreter(std::vector<T>, Compare = {}) -> Discreter<T, Compare>;
template<class Range, class Compare = std::less<>>
Discreter(const Range&, Compare = {}) -> Discreter<typename Range::value_type, Compare>;
namespace discreteDetail {
    template<class> struct IsDiscreter : std::false_type {};
    template<class T, class Compare> struct IsDiscreter<Discreter<T, Compare>> : std::true_type {};
}
// Sorted lvalue bases are borrowed; rvalues are owned by the operation.
template<class List, class Compare = std::less<>>
auto discreteFrom(List&& values, Compare cmp = {}) {
    return seq::Op{[values = std::tuple<List>(std::forward<List>(values)), cmp = std::move(cmp)](const auto& input) mutable {
        const auto& basis = std::get<0>(values);
        assert(basis.size() < std::size_t(std::numeric_limits<int>::max()));
        std::vector<int> result(input.size());
        for (std::size_t i = 0; i < input.size(); ++i) {
            if constexpr (discreteDetail::IsDiscreter<std::decay_t<List>>::value) result[i] = basis.rankOf(seq::listValue(input, i));
            else result[i] = int(std::lower_bound(basis.begin(), basis.end(), seq::listValue(input, i), std::ref(cmp)) - basis.begin());
        }
        return result;
    }};
}
template<class T, class Compare = std::less<>>
class PersistentTree {
    struct Node { int count = 0, left = 0, right = 0; };
    const int n;
    const Discreter<T, Compare> disc;
    std::vector<Node> nodes;
    std::vector<int> roots;
    static int checkedSize(const std::vector<T>& input) {
        assert(input.size() < std::size_t(std::numeric_limits<int>::max()));
        return int(input.size());
    }
    int pushBack(int original, int l, int r, int rank) {
        int node = int(nodes.size());
        nodes.push_back(nodes[original]);
        ++nodes[node].count;
        if (r - l > 1) {
            int mid = int((unsigned(l) + unsigned(r)) / 2);
            if (rank < mid) nodes[node].left = pushBack(nodes[node].left, l, mid, rank);
            else nodes[node].right = pushBack(nodes[node].right, mid, r, rank);
        }
        return node;
    }
    int query(int x, int y, int l, int r, int tl, int tr) const {
        if (tl <= l && r <= tr) return nodes[y].count - nodes[x].count;
        int mid = int((unsigned(l) + unsigned(r)) / 2), result = 0;
        if (tl < mid) result += query(nodes[x].left, nodes[y].left, l, mid, tl, tr);
        if (mid < tr) result += query(nodes[x].right, nodes[y].right, mid, r, tl, tr);
        return result;
    }
    int rankRange(int l, int r, int tl, int tr) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r || tl == tr) return 0;
        return query(roots[l], roots[r], 0, disc.size(), tl, tr);
    }
public:
    explicit PersistentTree(const std::vector<T>& input, Compare cmp = {})
        : n(checkedSize(input)), disc(input, std::move(cmp)), nodes(1), roots(std::size_t(n) + 1) {
        if (n == 0) return;
        int levels = 0;
        for (unsigned width = 1; width < unsigned(disc.size()); width *= 2) ++levels;
        assert(std::size_t(n) <= std::size_t(std::numeric_limits<int>::max() - 1) / (levels + 1));
        nodes.reserve(1 + std::size_t(n) * (levels + 1));
        for (int i = 0; i < n; ++i) roots[i + 1] = pushBack(roots[i], 0, disc.size(), disc.rankOf(input[i]));
    }
    int size() const { return n; }
    // Position [l,r), value [tl,tr) in comparator order.
    int range(int l, int r, const T& tl, const T& tr) const {
        assert(!disc.comparator()(tr, tl));
        return rankRange(l, r, disc.rankOf(tl), disc.rankOf(tr));
    }
    int countLess(int l, int r, const T& value) const {
        return rankRange(l, r, 0, disc.rankOf(value));
    }
    int countLessEqual(int l, int r, const T& value) const {
        return rankRange(l, r, 0, disc.upperRankOf(value));
    }
    typename Discreter<T, Compare>::ConstReference upTo(int l, int r, int k) const & {
        assert(0 <= l && l < r && r <= n && 0 <= k && k < r - l);
        int x = roots[l], y = roots[r], lo = 0, hi = disc.size();
        while (hi - lo > 1) {
            int mid = int((unsigned(lo) + unsigned(hi)) / 2);
            int leftCount = nodes[nodes[y].left].count - nodes[nodes[x].left].count;
            if (k < leftCount) { x = nodes[x].left; y = nodes[y].left; hi = mid; }
            else { k -= leftCount; x = nodes[x].right; y = nodes[y].right; lo = mid; }
        }
        return disc.at(lo);
    }
    T upTo(int l, int r, int k) const && {
        return static_cast<const PersistentTree&>(*this).upTo(l, r, k);
    }
};

}
#undef call
int main(){
    std::mt19937 rng(113);const int n=100000;std::vector<int> data(n);for(auto& x:data)x=rng()%1000000;
    compare("build",[&]{Before::PersistentTree<int> tree(data);return std::uint64_t(tree.upTo(0,n,n/2));},[&]{PersistentTree tree(data);return std::uint64_t(tree.upTo(0,n,n/2));});
    Before::PersistentTree<int> old(data);PersistentTree current(data);struct Query{int l,r,tl,tr,k;};std::vector<Query> queries(100000);for(auto& q:queries){q.l=rng()%n;q.r=q.l+1+rng()%(n-q.l);q.tl=rng()%1000000;q.tr=q.tl+1+rng()%(1000001-q.tl);q.k=rng()%(q.r-q.l);}
    compare("range-count",[&]{std::uint64_t sum=0;for(auto q:queries)sum+=old.range(q.l,q.r,q.tl,q.tr);return sum;},[&]{std::uint64_t sum=0;for(auto q:queries)sum+=current.range(q.l,q.r,q.tl,q.tr);return sum;});
    compare("kth-query",[&]{std::uint64_t sum=0;for(auto q:queries)sum+=old.upTo(q.l,q.r,q.k);return sum;},[&]{std::uint64_t sum=0;for(auto q:queries)sum+=current.upTo(q.l,q.r,q.k);return sum;});
}
