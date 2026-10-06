#include "../../../../../src/DataStructures/BaseDataStructures/HashMap/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using _hashmap::Impl;

struct ThrowValue {
    inline static bool throwAssign = false;
    int value = 0;
    ThrowValue& operator=(ThrowValue&& other) {
        if (throwAssign)
            throw std::runtime_error("fixture");
        value = other.value;
        return *this;
    }
};

int main() {
    using Map = Impl<int, 101, 50000>;
    static_assert(std::is_same_v<decltype(std::declval<const Map&>()(0)), const int&>);
    static_assert(std::is_same_v<decltype(*std::declval<Map::Iterator>()), std::pair<u64, int>>);
    for (int batch = 0; batch < 30; ++batch) {
        std::vector<Map> maps(5);
        std::vector<std::unordered_map<u64, int>> oracle(5);
        for (int iteration = 0; iteration < 5000; ++iteration) {
            int id = randomInt(0, 4), action = randomInt(0, 9);
            u64 key = testRng() % 200;
            if (action < 6) {
                int amount = randomInt(-50, 50);
                maps[id][key] += amount;
                oracle[id][key] += amount;
            } else if (action == 6) {
                maps[id].clear();
                oracle[id].clear();
            } else if (action == 7) {
                int other = randomInt(0, 4);
                if (other != id) {
                    maps[id] = std::move(maps[other]);
                    oracle[id] = std::move(oracle[other]);
                    oracle[other].clear();
                } else {
                    Map& source = maps[id];
                    maps[id] = std::move(source);
                }
            } else {
                const auto& map = maps[id];
                auto found = oracle[id].find(key);
                auto value = map(key);
                CHECK(value == (found == oracle[id].end() ? 0 : found->second));
                std::unordered_set<u64> visited;
                for (auto [storedKey, storedValue] : map) {
                    CHECK(visited.insert(storedKey).second);
                    CHECK(oracle[id].at(storedKey) == storedValue);
                }
                CHECK(visited.size() == oracle[id].size());
            }
        }
    }
    // Fill exactly Capa slots; exceeding the pool is a caller precondition violation.
    for (int batch = 0; batch < 500; ++batch) {
        Impl<int, 1, 8> map;
        for (u64 x = 0; x < 8; ++x)
            map[x] = int(x);
        CHECK(!map(9));
        map.clear();
        CHECK(map.begin() == map.end());
        map[99] = 7;
        CHECK(map(99) == 7);
    }
    {
        Impl<int, 10007, 8> map;
        for (int repeat = 0; repeat < 100; ++repeat) {
            map[repeat] = repeat;
            map.clear();
        }
        CHECK(map.begin() == map.end());
    }
    {
        Impl<std::unique_ptr<int>, 17, 20> map, interleaved;
        map[1] = std::make_unique<int>(3);
        interleaved[1] = std::make_unique<int>(9);
        map[2] = std::make_unique<int>(4);
        ++*map[1];
        CHECK(*map[1] == 4 && *map[2] == 4 && *interleaved[1] == 9);
        auto moved(std::move(map));
        CHECK(map.begin() == map.end() && *moved[2] == 4);
        map[3] = std::make_unique<int>(5);
        CHECK(*map[3] == 5);
    }
    std::weak_ptr<int> oldValue;
    {
        Impl<std::shared_ptr<int>, 17, 8> a, b;
        a[1] = std::make_shared<int>(7);
        oldValue = a(1);
        a.clear();
        CHECK(!oldValue.expired() && !a(1));
        b[1] = std::make_shared<int>(9);
    }
    CHECK(oldValue.expired());
    {
        Impl<ThrowValue, 17, 8> map;
        map[1].value = 3;
        ThrowValue::throwAssign = true;
        bool caught = false;
        try {
            map[2];
        } catch (const std::runtime_error&) {
            caught = true;
        }
        CHECK(caught && map[1].value == 3);
        ThrowValue::throwAssign = false;
        CHECK(map[2].value == 0);
        map[2].value = 4;
    }
#ifndef __clang__
    static_assert(sizeof(HashMap<int>) > 0);
    HashMap<int, 100, 200> alias;
#else
    // Clang C++17 cannot use std::sqrt to form the compile-time random modulus.
    Impl<int, 97, 200> alias;
#endif
    alias[~u64(0)] = 17;
    CHECK(alias(~u64(0)) == 17);
    {
        Map map;
        CHECK(!map(7) && map.begin() == map.end());
        map[7] = 0;
        auto saved = std::as_const(map)(7);
        CHECK(saved == 0 && !map(8));
        for (auto [key, value] : map) {
            CHECK(key == 7 && value == 0);
            value = 99;
            CHECK(map(key) == 0); // Original iterator returns a copy.
        }
        map[7] = 9;
        CHECK(saved == 0 && map(7) == 9);
        map.clear();
        CHECK(saved == 0 && !map(7));
    }
    {
        Impl<bool, 17, 8> map;
        map[1] = false;
        auto value = map(1);
        CHECK(!value && !map(2));
        int cnt = 0; for (auto [key, val] : map) { CHECK(key == 1 and !val); ++cnt; } CHECK(cnt == 1);
    }
    std::cout << "HashMap 150K interleaved-owner oracle, moves/clears/collisions/full pool, Original iterator copies, resource reset, default-value lookup PASS\n";
}
