#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
#include <random>
#include <string>
#include <utility>

// Snapshot of the Final immediately before this refactor, not Original.hpp.
namespace Previous {
#include "BenchmarkBefore.hpp"
}
// Fixed parameters make separate process runs and saved baselines comparable.
inline constexpr int fixedBases[] = {911382323, 972663749};
using Before = Previous::StringHashImpl<2, fixedBases, _strhash::p>;
using Current = _strhash::Impl<2, fixedBases, _strhash::p>;
using U64 = unsigned long long;

// Each cold sample gets its own template instance and initially empty power cache.
// Disable adaptive batching: one validation, one warmup, then seven cold instances.
template <int I>
inline const int bases[] = {fixedBases[0], fixedBases[1]};

template <int I, bool Query, bool New>
U64 coldBuild(const std::string &text) {
    using H = std::conditional_t<New, _strhash::Impl<2, bases<I>, _strhash::p>,
                                Previous::StringHashImpl<2, bases<I>, _strhash::p>>;
    H hash(text);
    asm volatile("" : : "g"(&hash) : "memory");
    if constexpr (Query) return hash.getU64(0);
    else return hash.size();
}

template <int Tag, bool Query, std::size_t... I>
void coldCompare(const char *name, const std::string &text, std::index_sequence<I...>) {
    U64 (*before[])(const std::string &) = {coldBuild<Tag + int(I), Query, false>...};
    U64 (*after[])(const std::string &) = {coldBuild<Tag + int(I), Query, true>...};
    int oldIndex = 0, newIndex = 0;
    compare(name, [&] { return before[oldIndex++](text); },
                  [&] { return after[newIndex++](text); }, false);
}

template <class H>
U64 querySum(const H &hash, const std::vector<std::pair<int, int>> &queries) {
    U64 sum = 0;
    for (auto [l, r] : queries) sum += hash.getU64(l, r);
    return sum;
}

int main() {
    std::mt19937 rng(97);
    std::string text(500000, 'a');
    for (auto &c : text) c += rng() % 26;
    auto suffix = "-" + std::to_string(fixedBases[0]) + "-" + std::to_string(fixedBases[1]);
    auto name = [&](const char *s) { return std::string(s) + suffix; };
    coldCompare<0, false>(name("cold-string-build").c_str(), text, std::make_index_sequence<9>{});
    coldCompare<9, true>(name("cold-build-full-query").c_str(), text, std::make_index_sequence<9>{});

    Before old(text);
    Current current(text);
    if (old.getU64(0) != current.getU64(0)) return 1; // Populate both shared caches.
    compare(name("warm-string-build").c_str(),
            [&] { Before hash(text); return hash.getU64(0); },
            [&] { Current hash(text); return hash.getU64(0); });
    compare(name("warm-c-string-build").c_str(),
            [&] { Before hash(text.c_str()); return hash.getU64(0); },
            [&] { Current hash(text.c_str()); return hash.getU64(0); });

    std::vector<std::pair<int, int>> queries(500000), shortQueries(500000);
    for (auto &[l, r] : queries) {
        l = rng() % text.size();
        r = l + 1 + rng() % (text.size() - l);
    }
    for (auto &[l, r] : shortQueries) {
        l = rng() % (text.size() - 64);
        r = l + rng() % 65; // Includes empty intervals.
    }
    compare(name("substring-queries").c_str(), [&] { return querySum(old, queries); },
                                              [&] { return querySum(current, queries); });
    compare(name("short-substring-queries").c_str(), [&] { return querySum(old, shortQueries); },
                                                    [&] { return querySum(current, shortQueries); });
    compare(name("warm-build-and-queries").c_str(),
            [&] { Before hash(text); return querySum(hash, queries); },
            [&] { Current hash(text); return querySum(hash, queries); });
}
