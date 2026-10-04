#define treemap beforeMap
#include "before.hpp"
#undef treemap
#define treemap afterMap
#include "after.hpp"
#undef treemap

#include <array>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <string>

namespace {
using Clock = std::chrono::steady_clock;
struct Hash {
    std::uint64_t value = 14695981039346656037ULL;
    void add(std::uint64_t x) { value = (value ^ x) * 1099511628211ULL; }
};
volatile std::uint64_t observed = 0;
void require(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
// Escape the live map object and its reachable storage to a compiler memory
// barrier. Old payload writes must remain before the timestamp even though
// logical erase/clear makes those slots inaccessible through the public API.
template<class T> inline void memoryBarrier(T& object) {
    __asm__ __volatile__("" : : "g"(std::addressof(object)) : "memory");
    std::atomic_signal_fence(std::memory_order_seq_cst);
}
struct Data {
    int n;
    std::vector<int> initial, eraseOrder, probes;
    std::array<std::uint64_t, 2> initialHashes{}; // bool / int.
    std::uint64_t eraseHash = 0, probeHash = 0;
};
Data makeData(int n) {
    Data data{n, {}, {}, {}, {}, 0, 0};
    Hash boolHash, intHash, erased;
    for (int i = 0; i < n; ++i) {
        data.initial.push_back(i);
        boolHash.add(static_cast<std::uint64_t>(i)); boolHash.add(1);
        intHash.add(static_cast<std::uint64_t>(i)); intHash.add(static_cast<std::uint64_t>(i + 1));
        erased.add(1);
    }
    std::mt19937_64 random(2026093017);
    std::shuffle(data.initial.begin(), data.initial.end(), random);
    data.eraseOrder = data.initial;
    std::shuffle(data.eraseOrder.begin(), data.eraseOrder.end(), random);
    data.probes = {0, 1, n / 4, n / 2, 3 * (n / 4), n - 1};
    data.probes.erase(std::remove_if(data.probes.begin(), data.probes.end(), [=](int key) { return key >= n; }), data.probes.end());
    std::sort(data.probes.begin(), data.probes.end());
    data.probes.erase(std::unique(data.probes.begin(), data.probes.end()), data.probes.end());
    Hash probeHash;
    for (int key : data.probes) { probeHash.add(static_cast<std::uint64_t>(key)); probeHash.add(0); }
    data.initialHashes = {boolHash.value, intHash.value};
    data.eraseHash = erased.value; data.probeHash = probeHash.value;
    return data;
}
struct Result { double totalMs, eraseNs; std::uint64_t checksum; };
template<class Map, class Value, bool Offline>
Result run(const Data& data, bool clearOnly) {
    Map map = [&]() {
        if constexpr (Offline) return Map(data.initial); // N registered candidates, all active.
        else return Map(data.n);
    }();
    for (int key : data.initial)
        require(map.insert(key, static_cast<Value>(key + 1)), "initial insert failed");
    require(map.size() == data.n, "initial size mismatch");
    Hash initial;
    int expectedKey = 0;
    for (auto entry : map) {
        require(entry.key == expectedKey && static_cast<Value>(entry.value) == static_cast<Value>(entry.key + 1),
                "initial payload/key traversal mismatch");
        initial.add(static_cast<std::uint64_t>(entry.key)); initial.add(static_cast<std::uint64_t>(entry.value));
        ++expectedKey;
    }
    require(expectedKey == data.n && initial.value == data.initialHashes[std::is_same_v<Value, bool> ? 0 : 1],
            "initial stream checksum mismatch");
    observed = observed ^ initial.value;
    Hash results;
    memoryBarrier(map);
    const auto start = Clock::now();
    if (clearOnly) map.clear();
    else for (int key : data.eraseOrder) results.add(map.erase(key));
    memoryBarrier(map);
    const auto stop = Clock::now();
    memoryBarrier(map); // The same map is still alive and observed after timing.

    require(map.empty() && map.size() == 0 && map.begin() == map.end(), "erase/clear did not empty map");
    if (!clearOnly) require(results.value == data.eraseHash, "erase return-stream checksum mismatch");
    for (int key : data.probes) {
        require(map(key) == Value{} && !map.contains(key), "missing read exposed old payload");
        require(static_cast<Value>(map[key]) == Value{}, "reused slot did not initialize default payload");
        require(map.contains(key), "default insertion lost membership");
    }
    require(map.size() == static_cast<int>(data.probes.size()), "probe insertion count mismatch");
    Hash probes;
    std::size_t index = 0;
    for (auto entry : map) {
        require(index < data.probes.size() && entry.key == data.probes[index] && static_cast<Value>(entry.value) == Value{},
                "reinserted default traversal mismatch");
        probes.add(static_cast<std::uint64_t>(entry.key)); probes.add(static_cast<std::uint64_t>(entry.value));
        ++index;
    }
    require(index == data.probes.size() && probes.value == data.probeHash, "probe result checksum mismatch");
    observed = observed ^ probes.value ^ results.value;
    const double ns = std::chrono::duration<double, std::nano>(stop - start).count();
    return {ns / 1000000.0, clearOnly ? 0 : ns / data.n, initial.value ^ probes.value ^ results.value};
}
Result dispatch(int kind, bool after, const Data& data, bool clearOnly) {
    switch (kind) {
        case 0:
            return after ? run<afterMap::TreeMap<int, bool>, bool, false>(data, clearOnly)
                         : run<beforeMap::TreeMap<int, bool>, bool, false>(data, clearOnly);
        case 1:
            return after ? run<afterMap::TreeMap<int, int>, int, false>(data, clearOnly)
                         : run<beforeMap::TreeMap<int, int>, int, false>(data, clearOnly);
        case 2:
            return after ? run<afterMap::TreeMapOff<int, bool>, bool, true>(data, clearOnly)
                         : run<beforeMap::TreeMapOff<int, bool>, bool, true>(data, clearOnly);
        default:
            return after ? run<afterMap::TreeMapOff<int, int>, int, true>(data, clearOnly)
                         : run<beforeMap::TreeMapOff<int, int>, int, true>(data, clearOnly);
    }
}
} // namespace

int main(int argc, char** argv) {
    try {
        int n = 1000000, rounds = 5;
        for (int i = 1; i < argc; ++i) {
            const std::string flag = argv[i];
            if (++i == argc) throw std::invalid_argument("missing option value");
            const int value = std::stoi(argv[i]);
            if (flag == "--n") n = value;
            else if (flag == "--rounds") rounds = value;
            else throw std::invalid_argument("unknown option");
        }
        require(n > 0 && n < std::numeric_limits<int>::max() && rounds > 0, "invalid n/rounds");
        const Data data = makeData(n);
        std::cout << "workload,backend,payload,variant,round,n,total_ms,erase_ns_per_op,checksum\n"
                  << std::fixed << std::setprecision(6);
        for (bool clearOnly : {false, true}) for (int kind = 0; kind < 4; ++kind) {
            std::array<std::vector<double>, 2> samples;
            const char* backend = kind < 2 ? "TreeMap" : "TreeMapOff";
            const char* payload = kind % 2 == 0 ? "bool" : "int";
            const char* workload = clearOnly ? "clear" : "eraseAll";
            std::cerr << workload << ' ' << backend << '<' << payload << "> starting\n";
            for (int round = -1; round < rounds; ++round) {
                // Alternate AB/BA within each combination; setup is never timed.
                const bool afterFirst = ((round + 1 + kind) & 1) != 0;
                for (bool after : {afterFirst, !afterFirst}) {
                    const auto result = dispatch(kind, after, data, clearOnly);
                    if (round < 0) continue;
                    samples[after].push_back(result.totalMs);
                    std::cout << workload << ',' << backend << ',' << payload << ',' << (after ? "after" : "before")
                              << ',' << round + 1 << ',' << n << ',' << result.totalMs << ',';
                    if (!clearOnly) std::cout << result.eraseNs;
                    std::cout << ',' << result.checksum << '\n';
                }
                std::cout.flush();
            }
            for (int version = 0; version < 2; ++version) {
                auto sample = samples[version]; std::sort(sample.begin(), sample.end());
                const double median = (sample[(sample.size() - 1) / 2] + sample[sample.size() / 2]) / 2;
                std::cerr << workload << ' ' << backend << '<' << payload << "> " << (version ? "after" : "before")
                          << std::fixed << std::setprecision(6) << " median total=" << median
                          << " ms, min=" << sample.front() << ", max=" << sample.back() << '\n';
            }
        }
        std::cerr << "All initial states, empty states, return hashes and zero-default slot reuse checks passed.\n"
                     "Single-call clear uses total ms only; very short samples approach clock/barrier overhead.\n";
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
