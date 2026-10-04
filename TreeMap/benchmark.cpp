#include "Final.hpp"

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
using Clock = std::chrono::steady_clock;
using PbdsTree = __gnu_pbds::tree<int, __gnu_pbds::null_type, std::less<int>,
    __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;
constexpr std::array<const char*, 5> containerNames{"TreeMap", "TreeMapOff", "stdSet", "stdMap", "PBDS"};
enum class Work { build, containsHit, containsMiss, erase, updateMixed, iterate, rankOf, keyAt, orderMixed };
constexpr std::array<const char*, 9> workNames{
    "build", "containsHit", "containsMiss", "erase", "updateMixed", "iterate", "rankOf", "keyAt", "orderMixed"};
struct Options {
    std::size_t n = 1000000, q = 1000000;
    int rounds = 5;
    std::uint64_t seed = 20260929;
    std::string summaryPath;
};
std::uint64_t parseNumber(const std::string& text) {
    if (text.empty() || text.front() == '-') throw std::invalid_argument("expected nonnegative integer");
    std::size_t consumed = 0;
    const auto value = std::stoull(text, &consumed);
    if (consumed != text.size()) throw std::invalid_argument("invalid integer: " + text);
    return value;
}
Options parseOptions(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string flag = argv[i];
        if (flag == "--help") {
            std::cout << "Usage: benchmark [--n 1000000] [--q 1000000] [--rounds 5] [--seed 20260929] [--summary FILE]\n"
                         "Random unique int keys; mapped values are bool. Offline universe has 2N keys.\n"
                         "Each workload gets one verified warmup and R measured rounds, with shuffled container order.\n"
                         "Build includes capacity allocation/offline universe copy+sort. Other initialization is excluded.\n"
                         "updateMixed has 2Q operations; orderMixed has 4Q. Other operation counts appear in CSV.\n"
                         "std::set/std::map order-statistics workloads are N/A, never simulated with linear scans.\n"
                         "Raw CSV: stdout. Progress: stderr. Summary CSV: --summary, or stderr if omitted.\n";
            std::exit(0);
        }
        if (++i == argc) throw std::invalid_argument("missing value for " + flag);
        const std::string value = argv[i];
        if (flag == "--summary") options.summaryPath = value;
        else if (flag == "--n") options.n = parseNumber(value);
        else if (flag == "--q") options.q = parseNumber(value);
        else if (flag == "--seed") options.seed = parseNumber(value);
        else if (flag == "--rounds") {
            const auto count = parseNumber(value);
            if (count > static_cast<std::uint64_t>(std::numeric_limits<int>::max()))
                throw std::invalid_argument("too many rounds");
            options.rounds = static_cast<int>(count);
        } else throw std::invalid_argument("unknown option: " + flag);
    }
    if (!options.n || !options.q || options.rounds <= 0
        || options.n > static_cast<std::size_t>(std::numeric_limits<int>::max()) / 4
        || options.q > std::numeric_limits<std::size_t>::max() / 4)
        throw std::invalid_argument("positive n/q/rounds required; n must fit generated int keys");
    return options;
}
struct Hash {
    std::uint64_t value = 14695981039346656037ULL;
    void add(std::uint64_t x) { value = (value ^ x) * 1099511628211ULL; }
};
bool mappedValue(int key) { return ((key / 2) & 1) != 0; }
struct Data {
    std::vector<int> initial, universe, hits, misses, rankKeys, orders, eraseKeys;
    std::vector<std::pair<int, int>> mixed;
};
Data makeData(const Options& options) {
    Data data;
    std::mt19937_64 random(options.seed);
    data.initial.resize(options.n);
    data.universe.resize(2 * options.n);
    for (std::size_t i = 0; i < data.universe.size(); ++i) data.universe[i] = static_cast<int>(2 * i);
    std::copy_n(data.universe.begin(), options.n, data.initial.begin());
    std::shuffle(data.initial.begin(), data.initial.end(), random);
    std::shuffle(data.universe.begin(), data.universe.end(), random);
    data.eraseKeys = data.initial;
    std::shuffle(data.eraseKeys.begin(), data.eraseKeys.end(), random);
    data.eraseKeys.resize(std::min(options.n, options.q));
    std::vector<int> active(options.n);
    for (std::size_t i = 0; i < options.n; ++i) active[i] = static_cast<int>(2 * i);
    data.hits.reserve(options.q); data.misses.reserve(options.q);
    data.rankKeys.reserve(options.q); data.orders.reserve(options.q); data.mixed.reserve(options.q);
    for (std::size_t i = 0; i < options.q; ++i) {
        data.hits.push_back(static_cast<int>(2 * (random() % options.n)));
        data.misses.push_back(static_cast<int>(2 * (random() % options.n) + 1));
        data.rankKeys.push_back(static_cast<int>(random() % (4 * options.n + 3)) - 1);
        data.orders.push_back(static_cast<int>(random() % options.n));
        const auto slot = random() % options.n;
        const int old = active[slot];
        const int next = old < static_cast<int>(2 * options.n)
            ? static_cast<int>(2 * (options.n + slot)) : static_cast<int>(2 * slot);
        data.mixed.emplace_back(old, next);
        active[slot] = next;
    }
    return data;
}

// Independent iterative segment tree: no production SBT/Fenwick/PBDS code.
class OrderOracle {
    std::size_t leafCount = 1;
    std::vector<std::uint32_t> tree;
public:
    explicit OrderOracle(std::size_t n) {
        while (leafCount < 2 * n) leafCount *= 2;
        tree.resize(2 * leafCount);
        std::fill_n(tree.begin() + static_cast<std::ptrdiff_t>(leafCount), n, 1);
        for (std::size_t p = leafCount - 1; p; --p) tree[p] = tree[2 * p] + tree[2 * p + 1];
    }
    bool present(std::size_t p) const { return tree[leafCount + p] != 0; }
    void set(std::size_t p, bool active) {
        p += leafCount; tree[p] = active;
        for (p /= 2; p; p /= 2) tree[p] = tree[2 * p] + tree[2 * p + 1];
    }
    std::uint32_t prefix(std::size_t end) const {
        std::uint32_t total = 0;
        for (std::size_t left = leafCount, right = leafCount + end; left < right; left /= 2, right /= 2) {
            if (left & 1) total += tree[left++];
            if (right & 1) total += tree[--right];
        }
        return total;
    }
    int select(std::uint32_t order) const {
        if (order >= tree[1]) throw std::logic_error("oracle order out of bounds");
        std::size_t p = 1;
        while (p < leafCount) {
            p *= 2;
            if (order >= tree[p]) { order -= tree[p]; ++p; }
        }
        return static_cast<int>(2 * (p - leafCount));
    }
};
std::size_t prefixCoordinate(int key, std::size_t n) {
    return key <= 0 ? 0 : std::min<std::size_t>(2 * n, (static_cast<std::size_t>(key) + 1) / 2);
}
struct State {
    std::vector<int> keys;
    std::uint64_t hash = 0;
};
State stateFromMask(const std::vector<unsigned char>& active) {
    State state;
    Hash hash;
    for (std::size_t i = 0; i < active.size(); ++i) if (active[i]) {
        const int key = static_cast<int>(2 * i);
        state.keys.push_back(key); hash.add(static_cast<std::uint64_t>(key));
    }
    state.hash = hash.value;
    return state;
}
struct Expected {
    std::array<std::uint64_t, 9> flows{};
    std::array<State, 3> states; // Initial, after erase, after constant-size mixed.
};
Expected makeExpected(const Options& options, const Data& data) {
    Expected expected;
    std::array<Hash, 9> flows;
    for (std::size_t i = 0; i < options.n; ++i) {
        flows[static_cast<int>(Work::build)].add(1);
        flows[static_cast<int>(Work::iterate)].add(2 * i);
    }
    for (std::size_t i = 0; i < options.q; ++i) {
        flows[static_cast<int>(Work::containsHit)].add(1);
        flows[static_cast<int>(Work::containsMiss)].add(0);
        flows[static_cast<int>(Work::updateMixed)].add(1);
        flows[static_cast<int>(Work::updateMixed)].add(1);
    }
    for (std::size_t i = 0; i < data.eraseKeys.size(); ++i) flows[static_cast<int>(Work::erase)].add(1);
    OrderOracle oracle(options.n);
    for (std::size_t i = 0; i < options.q; ++i) {
        flows[static_cast<int>(Work::rankOf)].add(oracle.prefix(prefixCoordinate(data.rankKeys[i], options.n)));
        flows[static_cast<int>(Work::keyAt)].add(static_cast<std::uint64_t>(oracle.select(data.orders[i])));
    }
    std::vector<unsigned char> active(2 * options.n, 0);
    std::fill_n(active.begin(), options.n, 1);
    expected.states[0] = stateFromMask(active);
    for (int key : data.eraseKeys) {
        if (!active[static_cast<std::size_t>(key / 2)]) throw std::logic_error("duplicate generated erase");
        active[static_cast<std::size_t>(key / 2)] = 0;
    }
    expected.states[1] = stateFromMask(active);
    std::fill(active.begin(), active.end(), 0); std::fill_n(active.begin(), options.n, 1);
    for (std::size_t i = 0; i < options.q; ++i) {
        const auto [old, next] = data.mixed[i];
        const auto oldPosition = static_cast<std::size_t>(old / 2), newPosition = static_cast<std::size_t>(next / 2);
        if (!oracle.present(oldPosition) || oracle.present(newPosition)
            || !active[oldPosition] || active[newPosition]) throw std::logic_error("invalid generated replacement");
        oracle.set(oldPosition, false); oracle.set(newPosition, true);
        active[oldPosition] = 0; active[newPosition] = 1;
        auto& flow = flows[static_cast<int>(Work::orderMixed)];
        flow.add(1); flow.add(1);
        flow.add(oracle.prefix(prefixCoordinate(data.rankKeys[i], options.n)));
        flow.add(static_cast<std::uint64_t>(oracle.select(data.orders[i])));
    }
    expected.states[2] = stateFromMask(active);
    for (std::size_t i = 0; i < active.size(); ++i)
        if (oracle.present(i) != bool(active[i])) throw std::logic_error("oracle final states disagree");
    for (std::size_t i = 0; i < flows.size(); ++i) expected.flows[i] = flows[i].value;
    return expected;
}
const State& endState(Work work, const Expected& expected) {
    if (work == Work::erase) return expected.states[1];
    if (work == Work::updateMixed || work == Work::orderMixed) return expected.states[2];
    return expected.states[0];
}
std::size_t operations(Work work, const Options& options) {
    if (work == Work::build || work == Work::iterate) return options.n;
    if (work == Work::erase) return std::min(options.n, options.q);
    if (work == Work::updateMixed) return 2 * options.q;
    if (work == Work::orderMixed) return 4 * options.q;
    return options.q;
}
bool supported(Work work, int container) {
    const bool orderStatistics = work == Work::rankOf || work == Work::keyAt || work == Work::orderMixed;
    return !orderStatistics || container == 0 || container == 1 || container == 4;
}

template<class Map, bool Offline>
struct Custom {
    static constexpr bool ranked = true, mapped = true;
    Map map;
    static Map make(const Data& data) {
        if constexpr (Offline) return Map(data.universe);
        else return Map(static_cast<int>(data.initial.size()));
    }
    explicit Custom(const Data& data) : map(make(data)) {}
    bool insert(int key) { return map.insert(key, mappedValue(key)); }
    bool erase(int key) { return map.erase(key); }
    bool contains(int key) const { return map.contains(key); }
    std::size_t size() const { return map.size(); }
    int rankOf(int key) const { return map.rankOf(key); }
    int keyAt(int order) const { return map.keyAt(order); }
    template<class F> void each(F&& function) {
        auto end = map.end();
        for (auto it = map.begin(); it != end; ++it) {
            auto entry = *it; function(entry.key, bool(entry.value));
        }
    }
};
struct StandardSet {
    static constexpr bool ranked = false, mapped = false;
    std::set<int> map;
    explicit StandardSet(const Data&) {}
    bool insert(int key) { return map.insert(key).second; }
    bool erase(int key) { return map.erase(key) != 0; }
    bool contains(int key) const { return map.find(key) != map.end(); }
    std::size_t size() const { return map.size(); }
    template<class F> void each(F&& function) { for (int key : map) function(key, false); }
};
struct StandardMap {
    static constexpr bool ranked = false, mapped = true;
    std::map<int, bool> map;
    explicit StandardMap(const Data&) {}
    bool insert(int key) { return map.try_emplace(key, mappedValue(key)).second; }
    bool erase(int key) { return map.erase(key) != 0; }
    bool contains(int key) const { return map.find(key) != map.end(); }
    std::size_t size() const { return map.size(); }
    template<class F> void each(F&& function) { for (const auto& entry : map) function(entry.first, entry.second); }
};
struct Pbds {
    static constexpr bool ranked = true, mapped = false;
    PbdsTree map;
    explicit Pbds(const Data&) {}
    bool insert(int key) { return map.insert(key).second; }
    bool erase(int key) { return map.erase(key) != 0; }
    bool contains(int key) const { return map.find(key) != map.end(); }
    std::size_t size() const { return map.size(); }
    int rankOf(int key) const { return static_cast<int>(map.order_of_key(key)); }
    int keyAt(int order) const { return *map.find_by_order(static_cast<std::size_t>(order)); }
    template<class F> void each(F&& function) { for (int key : map) function(key, false); }
};
struct Result { double ns; std::uint64_t flow, finalHash; std::size_t finalSize; };
template<class Adapter>
Result measure(Work work, const Options& options, const Data& data, const Expected& expected) {
    std::optional<Adapter> map;
    if (work != Work::build) {
        map.emplace(data);
        for (int key : data.initial) if (!map->insert(key)) throw std::runtime_error("initial construction failed");
    }
    Hash flow;
    const auto start = Clock::now();
    switch (work) {
        case Work::build:
            map.emplace(data);
            for (int key : data.initial) flow.add(map->insert(key));
            break;
        case Work::containsHit:
            for (int key : data.hits) flow.add(map->contains(key));
            break;
        case Work::containsMiss:
            for (int key : data.misses) flow.add(map->contains(key));
            break;
        case Work::erase:
            for (int key : data.eraseKeys) flow.add(map->erase(key));
            break;
        case Work::updateMixed:
            for (auto [old, next] : data.mixed) { flow.add(map->erase(old)); flow.add(map->insert(next)); }
            break;
        case Work::iterate:
            map->each([&](int key, bool) { flow.add(static_cast<std::uint64_t>(key)); });
            break;
        case Work::rankOf:
            if constexpr (Adapter::ranked) for (int key : data.rankKeys) flow.add(map->rankOf(key));
            else throw std::logic_error("unsupported rankOf benchmark");
            break;
        case Work::keyAt:
            if constexpr (Adapter::ranked) for (int order : data.orders) flow.add(map->keyAt(order));
            else throw std::logic_error("unsupported keyAt benchmark");
            break;
        case Work::orderMixed:
            if constexpr (Adapter::ranked) for (std::size_t i = 0; i < options.q; ++i) {
                flow.add(map->erase(data.mixed[i].first)); flow.add(map->insert(data.mixed[i].second));
                flow.add(map->rankOf(data.rankKeys[i])); flow.add(map->keyAt(data.orders[i]));
            } else throw std::logic_error("unsupported orderMixed benchmark");
            break;
    }
    const auto stop = Clock::now();
    if (flow.value != expected.flows[static_cast<int>(work)])
        throw std::runtime_error("result-stream hash differs from independent oracle");
    const State& state = endState(work, expected);
    if (map->size() != state.keys.size()) throw std::runtime_error("final size mismatch");
    std::size_t position = 0;
    Hash final;
    map->each([&](int key, bool value) {
        if (position >= state.keys.size() || key != state.keys[position])
            throw std::runtime_error("final ordered key traversal mismatch");
        if constexpr (Adapter::mapped)
            if (value != mappedValue(key)) throw std::runtime_error("final mapped value mismatch");
        ++position; final.add(static_cast<std::uint64_t>(key));
    });
    if (position != state.keys.size() || final.value != state.hash)
        throw std::runtime_error("final traversal length/hash mismatch");
    return {std::chrono::duration<double, std::nano>(stop - start).count() / operations(work, options),
            flow.value, final.value, position};
}
Result dispatch(int container, Work work, const Options& options, const Data& data, const Expected& expected) {
    switch (container) {
        case 0: return measure<Custom<TreeMap<int, bool>, false>>(work, options, data, expected);
        case 1: return measure<Custom<TreeMapOff<int, bool>, true>>(work, options, data, expected);
        case 2: return measure<StandardSet>(work, options, data, expected);
        case 3: return measure<StandardMap>(work, options, data, expected);
        case 4: return measure<Pbds>(work, options, data, expected);
        default: throw std::logic_error("unknown container");
    }
}
using Samples = std::array<std::array<std::vector<double>, 5>, 9>;
void summarize(std::ostream& output, const Options& options, Samples samples) {
    output << "workload,container,n,q,operations,rounds,median_ns,min_ns,max_ns,set_over_current,map_over_current,pbds_over_current,status\n"
           << std::fixed << std::setprecision(3);
    for (std::size_t w = 0; w < samples.size(); ++w) {
        std::array<double, 5> medians{};
        for (std::size_t c = 0; c < containerNames.size(); ++c) {
            auto& times = samples[w][c];
            std::sort(times.begin(), times.end());
            if (!times.empty()) medians[c] = (times[(times.size() - 1) / 2] + times[times.size() / 2]) / 2;
        }
        for (std::size_t c = 0; c < containerNames.size(); ++c) {
            output << workNames[w] << ',' << containerNames[c] << ',' << options.n << ',' << options.q << ','
                   << operations(static_cast<Work>(w), options) << ',';
            if (samples[w][c].empty()) { output << "0,,,,,,,N/A\n"; continue; }
            output << samples[w][c].size() << ',' << medians[c] << ',' << samples[w][c].front() << ','
                   << samples[w][c].back() << ',';
            if (medians[2] > 0) output << medians[2] / medians[c];
            output << ',';
            if (medians[3] > 0) output << medians[3] / medians[c];
            output << ',';
            if (medians[4] > 0) output << medians[4] / medians[c];
            output << ",ok\n";
        }
    }
}
} // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parseOptions(argc, argv);
        std::cerr << "Generating random streams and independent segment-tree oracle: N=" << options.n
                  << " Q=" << options.q << " offline universe=" << 2 * options.n << '\n';
        const Data data = makeData(options);
        const Expected expected = makeExpected(options, data);
        Samples samples;
        std::mt19937_64 sequence(options.seed ^ 0x90d51cd784e42b73ULL);
        std::cout << "workload,container,round,n,q,operations,ns_per_op,result_hash,final_size,final_hash,status\n"
                  << std::fixed << std::setprecision(3);
        for (std::size_t w = 0; w < workNames.size(); ++w) {
            const auto work = static_cast<Work>(w);
            std::vector<int> order;
            for (int c = 0; c < static_cast<int>(containerNames.size()); ++c) {
                if (supported(work, c)) order.push_back(c);
                else std::cout << workNames[w] << ',' << containerNames[c] << ",0," << options.n << ','
                               << options.q << ',' << operations(work, options) << ",,,,,N/A\n";
            }
            for (int round = -1; round < options.rounds; ++round) {
                std::shuffle(order.begin(), order.end(), sequence);
                std::cerr << '[' << workNames[w] << "] "
                          << (round < 0 ? "warmup" : "round " + std::to_string(round + 1) + "/" + std::to_string(options.rounds))
                          << " beginning\n";
                for (int c : order) {
                    const Result result = dispatch(c, work, options, data, expected);
                    std::cerr << "  " << containerNames[c] << " verified: " << std::fixed << std::setprecision(3)
                              << result.ns << " ns/op\n";
                    if (round < 0) continue;
                    samples[w][c].push_back(result.ns);
                    std::cout << workNames[w] << ',' << containerNames[c] << ',' << round + 1 << ',' << options.n
                              << ',' << options.q << ',' << operations(work, options) << ',' << result.ns << ','
                              << result.flow << ',' << result.finalSize << ',' << result.finalHash << ",ok\n";
                }
                std::cout.flush();
            }
        }
        if (options.summaryPath.empty()) summarize(std::cerr, options, samples);
        else {
            std::ofstream summary(options.summaryPath);
            if (!summary) throw std::runtime_error("cannot open summary output: " + options.summaryPath);
            summarize(summary, options, samples);
            if (!summary) throw std::runtime_error("failed writing summary output");
        }
        std::cerr << "All warmups and measured runs verified against independent result streams and complete final states.\n"
                     "Ratios are reference median / current median; greater than 1 means current is faster.\n";
    } catch (const std::exception& error) {
        std::cerr << "benchmark failed: " << error.what() << '\n';
        return 1;
    }
}
