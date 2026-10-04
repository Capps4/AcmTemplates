#ifndef PROFILE_HEADER
#define PROFILE_HEADER "../../treeMap.hpp"
#endif
#ifndef PROFILE_NAMESPACE
#define PROFILE_NAMESPACE _treemap
#endif
#ifndef PROFILE_LABEL
#define PROFILE_LABEL "current"
#endif
#include PROFILE_HEADER

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
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

using Online = PROFILE_NAMESPACE::TreeMap<int, bool>;
using Offline = PROFILE_NAMESPACE::TreeMapOff<int, bool>;
using Clock = std::chrono::steady_clock;
using U64 = std::uint64_t;
volatile U64 observedChecksum = 0;

void require(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
U64 mix(U64 value) {
    value += 0x9e3779b97f4a7c15ULL;
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}
void fold(U64& hash, U64 answer, std::size_t index) { hash += (answer + 1) * (index + 1); }
template<class Function>
double measure(Function function) {
    std::atomic_signal_fence(std::memory_order_seq_cst);
    const auto start = Clock::now();
    function();
    std::atomic_signal_fence(std::memory_order_seq_cst);
    const auto stop = Clock::now();
    return std::chrono::duration<double, std::nano>(stop - start).count();
}

struct Fenwick {
    std::vector<std::uint32_t> tree;
    unsigned step = 1;
    explicit Fenwick(int size) : tree(static_cast<std::size_t>(size) + 1) {
        while (step <= static_cast<unsigned>(size) / 2) step <<= 1;
    }
    void add(int coordinate, int delta) {
        for (std::size_t p = static_cast<std::size_t>(coordinate) + 1; p < tree.size(); p += p & -p)
            tree[p] += delta;
    }
    std::uint32_t prefix(int coordinate) const {
        std::uint32_t result = 0;
        for (unsigned p = static_cast<unsigned>(coordinate); p; p -= p & -p) result += tree[p];
        return result;
    }
    int select(std::uint32_t rank) const {
        unsigned position = 0;
        for (unsigned jump = step; jump; jump >>= 1) {
            const unsigned next = position + jump;
            if (next < tree.size() && tree[next] <= rank) { position = next; rank -= tree[next]; }
        }
        return static_cast<int>(position); // Zero-based target coordinate.
    }
    void build(const std::vector<unsigned char>& active) {
        // tree must initially be zero; allocation/zeroing occurs outside this timer.
        for (std::size_t p = 1; p < tree.size(); ++p) {
            tree[p] += active[p - 1];
            const auto next = p + (p & -p);
            if (next < tree.size()) tree[next] += tree[p];
        }
    }
};

enum Kind : unsigned char { Contains, Rank, Select, Insert, Erase };
struct Operation { int coordinate, key, order; Kind kind; };
struct Trace {
    std::vector<Operation> operations;
    std::vector<unsigned char> finalActive;
    std::vector<std::uint32_t> finalBit;
    U64 resultHash = 0, stateHash = 0;
    int finalSize = 0, actualInserts = 0, actualErases = 0;
};

struct Data {
    int n, universe;
    std::vector<int> sorted, candidates, insertCoordinates, insertKeys, queryCoordinates, queryKeys, orders;
    std::vector<unsigned char> active;
    Fenwick initialBit;
    Trace basic, allFive;
    U64 candidateHash = 0, sortedHash = 0, stateHash = 0;
    U64 containsHash = 0, rankHash = 0, selectKeyHash = 0, selectCoordinateHash = 0, lowerBoundHash = 0;
    Data(int count, int capacity) : n(count), universe(capacity), sorted(capacity), active(capacity), initialBit(capacity) {
        for (int p = 0; p < universe; ++p) sorted[p] = 2 * p + 3;
        candidates = sorted;
        std::mt19937_64 random(0x20260930ULL);
        std::shuffle(candidates.begin(), candidates.end(), random);
        insertKeys.assign(candidates.begin(), candidates.begin() + n);
        for (int key : insertKeys) { int p = (key - 3) / 2; insertCoordinates.push_back(p); active[p] = 1; }
        initialBit.build(active);
        std::vector<int> prefix(static_cast<std::size_t>(universe) + 1), orderedCoordinates;
        orderedCoordinates.reserve(n);
        for (int p = 0; p < universe; ++p) {
            prefix[p + 1] = prefix[p] + active[p];
            if (active[p]) { stateHash += mix(sorted[p]); orderedCoordinates.push_back(p); }
            fold(candidateHash, candidates[p], p); fold(sortedHash, sorted[p], p);
        }
        for (int i = 0; i < n; ++i) {
            const int p = static_cast<int>(random() % universe), order = static_cast<int>(random() % n);
            queryCoordinates.push_back(p); queryKeys.push_back(sorted[p]); orders.push_back(order);
            fold(containsHash, active[p], i); fold(rankHash, prefix[p], i); fold(lowerBoundHash, p, i);
            fold(selectCoordinateHash, orderedCoordinates[order], i);
            fold(selectKeyHash, sorted[orderedCoordinates[order]], i);
        }
        basic = makeTrace(false, random);
        allFive = makeTrace(true, random);
    }
    Trace makeTrace(bool advanced, std::mt19937_64& random) const {
        Trace trace;
        trace.operations.reserve(n);
        auto live = active;
        Fenwick bit = initialBit;
        int count = n;
        for (int i = 0; i < n; ++i) {
            const int p = static_cast<int>(random() % universe);
            Kind kind;
            if (advanced) kind = static_cast<Kind>(random() % 5);
            else { unsigned draw = random() % 4; kind = draw < 2 ? Contains : draw == 2 ? Insert : Erase; }
            const int order = kind == Select ? static_cast<int>(random() % count) : 0;
            trace.operations.push_back({p, sorted[p], order, kind});
            U64 answer = 0;
            switch (kind) {
            case Contains: answer = live[p]; break;
            case Rank: answer = bit.prefix(p); break;
            case Select: answer = sorted[bit.select(order)]; break;
            case Insert:
                answer = !live[p];
                if (answer) { live[p] = 1; bit.add(p, 1); ++count; ++trace.actualInserts; }
                break;
            case Erase:
                answer = live[p];
                if (answer) { live[p] = 0; bit.add(p, -1); --count; ++trace.actualErases; }
                break;
            }
            fold(trace.resultHash, answer, i);
        }
        trace.finalSize = count;
        for (int p = 0; p < universe; ++p) if (live[p]) trace.stateHash += mix(sorted[p]);
        trace.finalActive = std::move(live);
        trace.finalBit = std::move(bit.tree);
        return trace;
    }
};

struct Recorder {
    struct Result { double ns; std::size_t items; U64 checksum; };
    int round = 0;
    std::map<std::string, std::vector<Result>> results;
    static void print(const std::string& round, const std::string& name, const Result& result) {
        std::cout << round << ',' << name << ',' << result.items << ',' << std::fixed << std::setprecision(6)
                  << result.ns / 1e6 << ',' << result.ns / result.items << ',' << result.checksum << ",1\n";
    }
    void record(const std::string& name, std::size_t items, double ns, U64 checksum) {
        observedChecksum = observedChecksum ^ checksum;
        results[name].push_back({ns, items, checksum});
        print(std::to_string(round), name, results[name].back());
        std::cout.flush();
    }
    void medians() {
        for (auto& item : results) {
            auto samples = item.second;
            std::sort(samples.begin(), samples.end(), [](const Result& a, const Result& b) { return a.ns < b.ns; });
            const std::size_t k = samples.size() / 2;
            auto result = samples[k];
            if (samples.size() % 2 == 0) result.ns = (samples[k - 1].ns + samples[k].ns) / 2;
            print("median", item.first, result);
        }
    }
};

template<class Map>
void checkState(const Map& map, int expectedSize, U64 expectedHash) {
    require(map.size() == expectedSize, "map final size mismatch");
    U64 hash = 0;
    for (auto [key, value] : map) { require(value, "map has a false payload"); hash += mix(key); }
    require(hash == expectedHash, "map final key hash mismatch");
}
void checkState(const std::set<int>& set, int expectedSize, U64 expectedHash) {
    require(set.size() == static_cast<std::size_t>(expectedSize), "std::set final size mismatch");
    U64 hash = 0;
    for (int key : set) hash += mix(key);
    require(hash == expectedHash, "std::set final key hash mismatch");
}

template<class Function>
void query(Recorder& recorder, const std::string& name, const Data& data, U64 expected, Function function) {
    U64 hash = 0;
    const double ns = measure([&] { for (int i = 0; i < data.n; ++i) fold(hash, function(i), i); });
    require(hash == expected, "query checksum mismatch");
    recorder.record(name, data.n, ns, hash);
}

template<class Map>
void basicMapMix(Recorder& recorder, const std::string& name, Map& map, const Data& data) {
    U64 hash = 0;
    const auto& trace = data.basic;
    const double ns = measure([&] {
        for (std::size_t i = 0; i < trace.operations.size(); ++i) {
            const auto& op = trace.operations[i];
            bool answer = op.kind == Contains ? map.contains(op.key)
                        : op.kind == Insert ? map.insert(op.key, true) : map.erase(op.key);
            fold(hash, answer, i);
        }
    });
    require(hash == trace.resultHash, "map basic mixed result checksum mismatch");
    checkState(map, trace.finalSize, trace.stateHash);
    recorder.record(name, trace.operations.size(), ns, hash);
}
void allFiveMapMix(Recorder& recorder, Offline& map, const Data& data) {
    U64 hash = 0;
    const auto& trace = data.allFive;
    const double ns = measure([&] {
        for (std::size_t i = 0; i < trace.operations.size(); ++i) {
            const auto& op = trace.operations[i];
            U64 answer = 0;
            switch (op.kind) {
            case Contains: answer = map.contains(op.key); break;
            case Rank: answer = map.rankOf(op.key); break;
            case Select: answer = map.keyAt(op.order); break;
            case Insert: answer = map.insert(op.key, true); break;
            case Erase: answer = map.erase(op.key); break;
            }
            fold(hash, answer, i);
        }
    });
    require(hash == trace.resultHash, "offline five-way result checksum mismatch");
    checkState(map, trace.finalSize, trace.stateHash);
    recorder.record("off_mixed20_each", trace.operations.size(), ns, hash);
}
void rawMix(Recorder& recorder, const std::string& name, const Data& data, const Trace& trace) {
    Fenwick bit = data.initialBit;
    auto live = data.active;
    U64 hash = 0;
    const double ns = measure([&] {
        for (std::size_t i = 0; i < trace.operations.size(); ++i) {
            const auto& op = trace.operations[i];
            U64 answer = 0;
            switch (op.kind) {
            case Contains: answer = live[op.coordinate]; break;
            case Rank: answer = bit.prefix(op.coordinate); break;
            case Select: answer = data.sorted[bit.select(op.order)]; break;
            case Insert:
                answer = !live[op.coordinate];
                if (answer) { live[op.coordinate] = 1; bit.add(op.coordinate, 1); }
                break;
            case Erase:
                answer = live[op.coordinate];
                if (answer) { live[op.coordinate] = 0; bit.add(op.coordinate, -1); }
                break;
            }
            fold(hash, answer, i);
        }
    });
    require(hash == trace.resultHash && live == trace.finalActive && bit.tree == trace.finalBit,
            "raw Fenwick mixed checksum/state mismatch");
    recorder.record(name, trace.operations.size(), ns, hash);
}

int main(int argc, char** argv) {
    try {
        const int n = argc > 1 ? std::stoi(argv[1]) : 1000000;
        const int universe = argc > 2 ? std::stoi(argv[2]) : 2000000;
        const int rounds = argc > 3 ? std::stoi(argv[3]) : 5;
        require(argc <= 4 && n > 0 && universe >= n && universe <= (std::numeric_limits<int>::max() - 3) / 2,
                "usage: profile [N=1000000 [U=2000000 [rounds=5]]], with 0 < N <= U");
        require(rounds >= 5, "at least five interleaved rounds are required");
        Data data(n, universe); // Includes both reference traces; all outside timers.
        Recorder recorder;
        std::cout << "# label=" << PROFILE_LABEL << " compiler=" << __VERSION__ << " N=" << n << " U=" << universe
                  << " rounds=" << rounds << " seed=0x20260930 value=bool\n"
                  << "# Query keys are registered universe keys; initial hit ratio is approximately N/U.\n"
                  << "# Constructor/copy/sort items count U candidates; SBT constructor counts N reserved slots.\n"
                  << "# Mixed basic: 50% contains,25% insert,25% erase; mixed20_each adds rank/select.\n"
                  << "# Basic changed inserts=" << data.basic.actualInserts << " erases=" << data.basic.actualErases
                  << "; five-way changed inserts=" << data.allFive.actualInserts << " erases=" << data.allFive.actualErases << '\n'
                  << "# Setup, copies for mixed state, validation, and destruction are outside timers. Query loops include a weighted checksum.\n"
                  << "# Raw Fenwick uses precomputed coordinates; linear build is bulk-initialization potential, not equivalent public insert semantics.\n"
                  << "# Component timings are diagnostic and not strictly additive because cache state differs.\n"
                  << "round,case,work_items,total_ms,ns_per_item,checksum,verified\n";
        using Group = std::pair<std::string, std::function<void()>>;
        std::vector<Group> groups;
        groups.push_back({"candidate_copy", [&] {
            std::optional<std::vector<int>> copied;
            const double ns = measure([&] { copied.emplace(data.candidates); });
            require(*copied == data.candidates, "candidate copy mismatch");
            recorder.record("candidate_copy", universe, ns, data.candidateHash);
        }});
        groups.push_back({"sort_unique", [&] {
            auto copied = data.candidates;
            const double ns = measure([&] {
                std::sort(copied.begin(), copied.end());
                copied.erase(std::unique(copied.begin(), copied.end()), copied.end());
            });
            require(copied == data.sorted, "sort/unique mismatch");
            recorder.record("sort_unique", universe, ns, data.sortedHash);
        }});
        for (bool presorted : {false, true}) groups.push_back({presorted ? "off_ctor_sorted" : "off_ctor_move", [&, presorted] {
            auto input = presorted ? data.sorted : data.candidates;
            std::optional<Offline> map;
            const double ns = measure([&] { map.emplace(std::move(input)); });
            require(map->empty(), "offline constructor inserted entries");
            map->insert(data.sorted.front(), true); map->insert(data.sorted.back(), true);
            require(map->contains(data.sorted.front()) && map->contains(data.sorted.back()), "offline moved constructor lost keys");
            require(map->keyAt(0) == data.sorted.front(), "offline moved constructor ordering mismatch");
            recorder.record(presorted ? "off_ctor_moved_presorted" : "off_ctor_moved_unsorted", universe, ns, data.sortedHash);
        }});
        groups.push_back({"binary_search", [&] {
            query(recorder, "std_lower_bound", data, data.lowerBoundHash, [&](int i) {
                return std::lower_bound(data.sorted.begin(), data.sorted.end(), data.queryKeys[i]) - data.sorted.begin();
            });
        }});
        groups.push_back({"sbt", [&] {
            std::optional<Online> map;
            double ns = measure([&] { map.emplace(n); });
            require(map->empty(), "SBT constructor inserted entries");
            recorder.record("sbt_ctor_fixed", n, ns, 0);
            U64 inserted = 0;
            ns = measure([&] { for (int key : data.insertKeys) inserted += map->insert(key, true); });
            require(inserted == static_cast<U64>(n), "SBT insertion count mismatch");
            checkState(*map, n, data.stateHash);
            recorder.record("sbt_insert_random", n, ns, inserted);
            query(recorder, "sbt_contains", data, data.containsHash, [&](int i) { return map->contains(data.queryKeys[i]); });
            query(recorder, "sbt_rankOf", data, data.rankHash, [&](int i) { return map->rankOf(data.queryKeys[i]); });
            query(recorder, "sbt_keyAt", data, data.selectKeyHash, [&](int i) { return map->keyAt(data.orders[i]); });
            map->reserve(universe); // Mixed population may temporarily exceed N; never timed.
            basicMapMix(recorder, "sbt_mixed50_25_25", *map, data);
        }});
        groups.push_back({"offline", [&] {
            std::optional<Offline> map;
            double ns = measure([&] { map.emplace(data.candidates); });
            require(map->empty(), "offline lvalue constructor inserted entries");
            recorder.record("off_ctor_lvalue_unsorted", universe, ns, data.sortedHash);
            U64 inserted = 0;
            ns = measure([&] { for (int key : data.insertKeys) inserted += map->insert(key, true); });
            require(inserted == static_cast<U64>(n), "offline insertion count mismatch");
            checkState(*map, n, data.stateHash);
            recorder.record("off_insert_random", n, ns, inserted);
            query(recorder, "off_contains", data, data.containsHash, [&](int i) { return map->contains(data.queryKeys[i]); });
            query(recorder, "off_rankOf", data, data.rankHash, [&](int i) { return map->rankOf(data.queryKeys[i]); });
            query(recorder, "off_keyAt", data, data.selectKeyHash, [&](int i) { return map->keyAt(data.orders[i]); });
            Offline forBasic = *map;
            basicMapMix(recorder, "off_mixed50_25_25", forBasic, data);
            allFiveMapMix(recorder, *map, data);
        }});
        groups.push_back({"raw_fenwick", [&] {
            Fenwick bit(universe);
            double ns = measure([&] { for (int coordinate : data.insertCoordinates) bit.add(coordinate, 1); });
            require(bit.tree == data.initialBit.tree, "raw add bit mismatch");
            recorder.record("raw_fenwick_add", n, ns, data.stateHash);
            query(recorder, "raw_fenwick_prefix", data, data.rankHash, [&](int i) { return bit.prefix(data.queryCoordinates[i]); });
            query(recorder, "raw_fenwick_select", data, data.selectCoordinateHash, [&](int i) { return bit.select(data.orders[i]); });
            Fenwick linear(universe);
            ns = measure([&] { linear.build(data.active); });
            require(linear.tree == data.initialBit.tree, "linear bit build mismatch");
            recorder.record("raw_fenwick_linear_build", universe, ns, data.stateHash);
            rawMix(recorder, "raw_fenwick_mixed50_25_25", data, data.basic);
            rawMix(recorder, "raw_fenwick_mixed20_each", data, data.allFive);
        }});
        groups.push_back({"std_set", [&] {
            std::set<int> set;
            U64 inserted = 0;
            double ns = measure([&] { for (int key : data.insertKeys) inserted += set.insert(key).second; });
            require(inserted == static_cast<U64>(n), "std::set insertion count mismatch");
            checkState(set, n, data.stateHash);
            recorder.record("std_set_insert_random", n, ns, inserted);
            query(recorder, "std_set_contains", data, data.containsHash, [&](int i) { return set.find(data.queryKeys[i]) != set.end(); });
            U64 hash = 0;
            ns = measure([&] {
                for (std::size_t i = 0; i < data.basic.operations.size(); ++i) {
                    const auto& op = data.basic.operations[i];
                    const bool answer = op.kind == Contains ? set.find(op.key) != set.end()
                                      : op.kind == Insert ? set.insert(op.key).second : set.erase(op.key) != 0;
                    fold(hash, answer, i);
                }
            });
            require(hash == data.basic.resultHash, "std::set mixed checksum mismatch");
            checkState(set, data.basic.finalSize, data.basic.stateHash);
            recorder.record("std_set_mixed50_25_25", data.basic.operations.size(), ns, hash);
        }});
        std::vector<std::size_t> order(groups.size());
        std::iota(order.begin(), order.end(), 0);
        std::mt19937 schedule(0xface1234);
        for (int round = 1; round <= rounds; ++round) {
            recorder.round = round;
            std::shuffle(order.begin(), order.end(), schedule);
            std::cout << "# round " << round << " group order:";
            for (auto index : order) std::cout << ' ' << groups[index].first;
            std::cout << '\n';
            for (auto index : order) groups[index].second();
        }
        recorder.medians();
        std::cout << "# ALL_CHECKS_PASSED observed_checksum=" << observedChecksum << '\n';
    } catch (const std::exception& error) {
        std::cerr << "ERROR: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
