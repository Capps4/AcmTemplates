#include "Legacy.hpp"
#include "Direct.hpp"
#include "Bridge.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>

using Clock = std::chrono::steady_clock;

void require(bool condition) {
    if (!condition)
        throw std::runtime_error("TreeMap sorting comparison mismatch");
}

template<int Method, class Key, class Compare>
__attribute__((noinline, aligned(64)))
void runSort(std::vector<Key>& a, Compare compare) {
    if constexpr (Method == 0)
        legacyMap::sortKeys(a, compare);
    if constexpr (Method == 1)
        a = std::move(a) | seq::sorted(compare);
    if constexpr (Method == 2) {
        a = std::move(a) | seq::sorted();
        if constexpr (std::is_same_v<Compare, std::greater<Key>>)
            std::reverse(a.begin(), a.end());
    }
    if constexpr (Method == 3)
        std::sort(a.begin(), a.end(), compare);
}

template<class Map, class Key, class Compare>
double construct(const std::vector<Key>& input,
                 const std::vector<Key>& distinct, Compare compare, bool moveInput) {
    std::vector<Key> working;
    if (moveInput)
        working = input;
    std::optional<Map> map;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    auto start = Clock::now();
    if (moveInput)
        map.emplace(std::move(working), compare);
    else
        map.emplace(input, compare);
    auto stop = Clock::now();
    std::atomic_signal_fence(std::memory_order_seq_cst);
    require(map->empty() && map->size() == 0);
    std::vector<Key> selected;
    const std::size_t count = std::min<std::size_t>(4096, distinct.size());
    for (std::size_t i = 0; i < count; ++i) {
        Key key = distinct[i * distinct.size() / count];
        require(map->insert(key, false));
        selected.push_back(key);
    }
    require(map->size() == static_cast<int>(count));
    for (int i = 0; i < map->size(); ++i) {
        require(map->keyAt(i) == selected[i]);
        require(map->rankOf(selected[i]) == i);
        auto value = (*map)(selected[i]);
        require(value.has_value() && !*value);
    }
    std::size_t index = 0;
    for (auto [key, value] : *map) {
        require(index < selected.size() && key == selected[index] && !value);
        ++index;
    }
    require(index == selected.size());
    return std::chrono::duration<double, std::nano>(stop - start).count();
}

template<int Method, class Key, class Compare>
__attribute__((noinline, aligned(64)))
double runConstructor(const std::vector<Key>& input,
                      const std::vector<Key>& distinct, Compare compare, bool moveInput) {
    if constexpr (Method == 0)
        return construct<legacyMap::TreeMapOff<Key, bool, Compare>>(input, distinct, compare, moveInput);
    if constexpr (Method == 1)
        return construct<directMap::TreeMapOff<Key, bool, Compare>>(input, distinct, compare, moveInput);
    if constexpr (Method == 2)
        return construct<bridgeMap::TreeMapOff<Key, bool, Compare>>(input, distinct, compare, moveInput);
}

template<class Key, class Compare>
void measure(const char* type, int n, int kind, Compare compare, const char* direction) {
    std::mt19937_64 random(203013 + n + kind * 31 + sizeof(Key) * 177);
    std::vector<Key> input(n);
    const char* distributions[] = {"range22", "fullWidth", "fewValues", "gaps", "ascending", "descending", "nearly"};
    for (int i = 0; i < n; ++i) {
        if (kind == 0)
            input[i] = static_cast<Key>(random() % (1 << 22));
        else if (kind == 1) {
            using U = std::make_unsigned_t<Key>;
            U bits = static_cast<U>(random());
            std::memcpy(&input[i], &bits, sizeof(Key));
        } else if (kind == 2)
            input[i] = static_cast<Key>(random() % 16) - 8;
        else if (kind == 3)
            input[i] = static_cast<Key>((random() % 256) << 16);
        else
            input[i] = static_cast<Key>(i - n / 2);
    }
    if (kind == 1 && n >= 2) {
        input[0] = std::numeric_limits<Key>::min();
        input[1] = std::numeric_limits<Key>::max();
    }
    if (kind == 5)
        std::reverse(input.begin(), input.end());
    if (kind == 6)
        for (int i = 0; i < n / 64 + 1; ++i)
            std::swap(input[random() % n], input[random() % n]);
    auto expected = input;
    std::sort(expected.begin(), expected.end(), compare);
    auto distinct = expected;
    distinct.erase(std::unique(distinct.begin(), distinct.end()), distinct.end());
    using SortFn = void (*)(std::vector<Key>&, Compare);
    std::array<std::pair<int, SortFn>, 4> sortMethods{{
        {0, runSort<0, Key, Compare>}, {1, runSort<1, Key, Compare>},
        {2, runSort<2, Key, Compare>}, {3, runSort<3, Key, Compare>}}};
    for (int round = -1; round < 7; ++round) {
        std::shuffle(sortMethods.begin(), sortMethods.end(), random);
        for (auto [method, function] : sortMethods) {
            double total = 0;
            int repeats = 0;
            do {
                auto a = input;
                std::atomic_signal_fence(std::memory_order_seq_cst);
                auto start = Clock::now();
                function(a, compare);
                auto stop = Clock::now();
                std::atomic_signal_fence(std::memory_order_seq_cst);
                require(a == expected);
                total += std::chrono::duration<double, std::nano>(stop - start).count();
                ++repeats;
            } while (total < 1000000 && repeats < 512);
            if (round >= 0)
                std::cout << "sort," << type << ',' << n << ',' << distributions[kind] << ','
                          << direction << ',' << method << ',' << round << ',' << repeats << ','
                          << total / repeats << '\n';
        }
    }
    if (n >= 1000000) {
        using ConstructorFn = double (*)(const std::vector<Key>&, const std::vector<Key>&, Compare, bool);
        std::array<std::pair<int, ConstructorFn>, 3> methods{{
            {0, runConstructor<0, Key, Compare>}, {1, runConstructor<1, Key, Compare>},
            {2, runConstructor<2, Key, Compare>}}};
        for (bool moveInput : {true, false}) {
            for (int round = -1; round < 7; ++round) {
                std::shuffle(methods.begin(), methods.end(), random);
                for (auto [method, function] : methods) {
                    double time = function(input, distinct, compare, moveInput);
                    if (round >= 0)
                        std::cout << (moveInput ? "ctorMove" : "ctorCopy") << ',' << type << ','
                                  << n << ',' << distributions[kind] << ',' << direction << ','
                                  << method << ',' << round << ",1," << time << '\n';
                }
            }
        }
    }
    std::cout.flush();
    std::cerr << type << ' ' << n << ' ' << distributions[kind] << ' ' << direction << " verified\n";
}

template<class Key>
void sweep(const char* type) {
    for (int kind = 0; kind < 7; ++kind) {
        measure<Key>(type, 1000000, kind, std::less<Key>{}, "asc");
        measure<Key>(type, 1000000, kind, std::greater<Key>{}, "desc");
    }
    for (int n : {128, 512, 4096})
        measure<Key>(type, n, 1, std::less<Key>{}, "asc");
}

int main() {
    std::cout << "work,type,n,distribution,direction,method,round,repeats,ns\n"
              << std::fixed << std::setprecision(3);
    sweep<int>("int");
    sweep<long long>("longLong");
    measure<int>("int", 2000000, 0, std::less<int>{}, "asc");
    measure<int>("int", 2000000, 1, std::less<int>{}, "asc");
}
