#include "../../../../../src/DataStructures/TreeDataStructures/TreeMap/code.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {
std::string testName;
int testStep = 0;
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(testName + " step " + std::to_string(testStep) + ": " + message);
}
template<class Exception, class Function>
void expectThrow(Function function) {
    bool caught = false;
    try { function(); } catch (const Exception&) { caught = true; }
    require(caught, "expected exception not thrown");
}
std::vector<int> keys(int radius) {
    std::vector<int> result;
    for (int key = -radius; key <= radius; ++key) result.push_back(key);
    result.push_back(0);
    std::mt19937 random(123);
    std::shuffle(result.begin(), result.end(), random);
    return result;
}

template<class Map, class Reference>
void checkQuery(const Map& map, const Reference& reference, int key) {
    auto found = reference.find(key);
    require(map.contains(key) == (found != reference.end()), "contains differs");
    auto value = map(key);
    require(value.has_value() == (found != reference.end()), "optional presence differs");
    if (value)
        require(*value == found->second, "read-only value differs");
    int expectedRank = static_cast<int>(std::distance(reference.begin(), reference.lower_bound(key)));
    require(map.rankOf(key) == expectedRank, "rankOf differs");
}

template<class Map, class Reference>
void snapshot(Map& map, const Reference& reference) {
    const Map& constant = map;
    static_assert(std::is_same<decltype(constant.size()), int>::value, "size must return int");
    static_assert(std::is_same<decltype(constant.rankOf(0)), int>::value, "rankOf must return int");
    static_assert(std::is_same<decltype(constant.keyAt(0)), int>::value, "keyAt must return a key by value");
    static_assert(std::is_same_v<decltype(constant(0)), std::optional<int>>, "lookup must return an optional copy");
    static_assert(std::is_same<decltype((*constant.begin()).key), const int&>::value, "iterator keys must be const");
    static_assert(!std::is_assignable<decltype(((*constant.begin()).value)), int>::value,
                  "const iteration must not permit value writeback");
    require(map.size() == static_cast<int>(reference.size()) && map.empty() == reference.empty(), "size/empty differs");
    auto actual = map.begin();
    int index = 0;
    for (const auto& expected : reference) {
        require(actual != map.end(), "forward iteration ended early");
        auto entry = *actual;
        require(entry.key == expected.first && entry.value == expected.second, "forward iterator entry differs");
        require(map.keyAt(index) == expected.first && constant.keyAt(index) == expected.first, "keyAt differs");
        auto saved = actual;
        ++actual;
        require((*saved).key == expected.first, "iterator copy shared a mutable traversal path");
        ++index;
    }
    require(actual == map.end(), "forward iteration produced extra entries");
    auto expected = reference.begin();
    for (auto [key, value] : constant) {
        require(expected != reference.end() && key == expected->first && value == expected->second,
                "const range traversal differs");
        ++expected;
    }
    require(expected == reference.end(), "const range traversal omitted entries");
    expectThrow<std::out_of_range>([&] { (void)constant.keyAt(-1); });
    expectThrow<std::out_of_range>([&] { (void)constant.keyAt(std::numeric_limits<int>::min()); });
    expectThrow<std::out_of_range>([&] { (void)constant.keyAt(map.size()); });
    expectThrow<std::out_of_range>([&] { (void)constant.keyAt(std::numeric_limits<int>::max()); });
    for (int key : {-999, -1, 0, 1, 999}) checkQuery(constant, reference, key);
}

struct Compare {
    int width = 1;
    bool descending = false;
    bool operator()(int a, int b) const { return descending ? a / width > b / width : a / width < b / width; }
};

template<class Map, class Comparator>
void randomCase(const std::string& name, Map& map, Comparator compare) {
    testName = name;
    std::map<int, int, Comparator> reference(compare);
    std::mt19937 random(91929);
    for (testStep = 0; testStep < 4000; ++testStep) {
        int key = static_cast<int>(random() % 129) - 64;
        int value = static_cast<int>(random() % 100000);
        switch (random() % 7) {
        case 0: case 1:
            require(map.insert(key, value) == reference.emplace(key, value).second, "insert result differs");
            break;
        case 2:
            require(map.insertOrAssign(key, value) == reference.insert_or_assign(key, value).second,
                    "insertOrAssign result differs");
            break;
        case 3: case 4:
            require(map.erase(key) == (reference.erase(key) != 0), "erase result differs");
            break;
        case 5: map[key] = value; reference[key] = value; break;
        case 6: checkQuery(map, reference, key); break;
        }
        require(map.size() == static_cast<int>(reference.size()), "mutation changed size incorrectly");
        checkQuery(map, reference, key);
        if (testStep % 31 == 0) snapshot(map, reference);
    }
    for (auto [key, value] : map) { value = -key; reference.find(key)->second = -key; }
    snapshot(map, reference);
    map.clear();
    reference.clear();
    snapshot(map, reference);
    map.insert(3, 30); reference.emplace(3, 30);
    snapshot(map, reference);
}

void differentialCases() {
    for (Compare compare : {Compare{1, false}, Compare{1, true}, Compare{3, true}}) {
        TreeMap<int, int, Compare> dynamic(compare);
        TreeMapOff<int, int, Compare> offline(keys(64), compare);
        randomCase("random/TreeMap", dynamic, compare);
        randomCase("random/TreeMapOff", offline, compare);
    }
    testName = "empty universe and equivalent keys";
    TreeMapOff<int, int> empty(std::vector<int>{});
    snapshot(empty, std::map<int, int>{});
    require(!empty(1) && !empty.contains(1) && !empty.erase(1), "unknown offline read/erase differs");
    expectThrow<std::out_of_range>([&] { empty[1] = 2; });
    expectThrow<std::out_of_range>([&] { empty.insert(1, 2); });
    expectThrow<std::out_of_range>([&] { empty.insertOrAssign(1, 2); });
    TreeMapOff<int, int, Compare> map(std::vector<int>{21}, Compare{10, false});
    require(map.insert(25, 1), "equivalent registered key rejected");
    map[28] = 2;
    require(map.size() == 1 && map.keyAt(0) == 25 && map(22) == 2, "equivalent update replaced original key");
    require(map.erase(29), "equivalent erase failed");
    map[27] = 3;
    require(map.keyAt(0) == 27, "reinsertion retained an old representative");
}

void capacityCases() {
    testName = "fixed capacity and recycling";
    TreeMap<int, int> zero(0);
    expectThrow<std::length_error>([&] { zero.insert(1, 1); });
    expectThrow<std::length_error>([&] { zero[1] = 1; });
    require(zero.empty(), "zero-capacity failure inserted an entry");
    zero.reserve(1); zero[1] = 1;
    expectThrow<std::length_error>([&] { zero[2] = 2; });
    TreeMap<int, int> map(7);
    std::map<int, int> reference;
    for (int key : {4, 2, 6, 1, 3, 5, 7}) { map.insert(key, key * 10); reference.emplace(key, key * 10); }
    expectThrow<std::length_error>([&] { map.insert(8, 80); });
    require(!map.insert(1, 999) && map(1) == 10, "full duplicate insertion changed value");
    require(!map.insertOrAssign(1, 11), "full existing assignment failed"); reference[1] = 11;
    map.erase(4); reference.erase(4); map.insert(8, 80); reference.emplace(8, 80);
    snapshot(map, reference);
    for (testStep = 0; testStep < 1800; ++testStep) {
        int removed = reference.begin()->first, added = testStep + 10;
        map.erase(removed); reference.erase(removed);
        require(map.insert(added, added * 10), "freed fixed-capacity slot was not reused");
        reference.emplace(added, added * 10);
        if (testStep % 47 == 0) snapshot(map, reference);
    }
    map.reserve(10);
    for (int key : {-3, -2, -1}) { map.insert(key, key); reference.emplace(key, key); }
    snapshot(map, reference);
    expectThrow<std::length_error>([&] { map.insert(-4, -4); });
    map.clear(); map.insert(9, 90);
    require(map.size() == 1 && map(9) == 90, "clear did not permit fixed-capacity reuse");
}

struct CountedDefault {
    static inline int constructed = 0;
    int number;
    CountedDefault() : number(0) { ++constructed; }
    explicit CountedDefault(int number) : number(number) {}
};

struct TrivialTemplateAssignment {
    static inline int assigned = 0;
    int number;
    TrivialTemplateAssignment() = default;
    TrivialTemplateAssignment(const TrivialTemplateAssignment&) = default;
    TrivialTemplateAssignment& operator=(const TrivialTemplateAssignment&) = default;
    template<class T> TrivialTemplateAssignment& operator=(T&& other) {
        ++assigned;
        number = other.number;
        return *this;
    }
};
static_assert(std::is_trivial_v<TrivialTemplateAssignment>);
static_assert(!std::is_trivially_move_assignable_v<TrivialTemplateAssignment>);

template<class Map>
void uniqueCase(Map& map) {
    for (int key : {4, 2, 6, 1, 3, 5, 7}) map.insert(key, std::make_unique<int>(key * 10));
    static_assert(std::is_same_v<decltype((*std::as_const(map).begin()).value), const std::unique_ptr<int>&>,
                  "const iteration must retain move-only references");
    require(*map[4] == 40 && !map.contains(99) && map.size() == 7, "move-only access differs");
    map.erase(4);
    for (auto [key, value] : map) require(*value == key * 10, "two-child erase detached payload from key");
    map[4] = std::make_unique<int>(44);
    require(*map[4] == 44, "move-only value reuse differs");
    map.clear();
    require(map.empty() && !map.contains(4), "move-only clear retained an entry");
}

void valueCases() {
    testName = "native vector values and default access";
    TreeMap<int, std::unique_ptr<int>> dynamic;
    TreeMapOff<int, std::unique_ptr<int>> offline(keys(10));
    uniqueCase(dynamic); uniqueCase(offline);
    const auto checkBool = [](auto& map) {
        static_assert(std::is_same<decltype(map[1]), std::vector<bool>::reference>::value,
                      "bool subscript must expose native vector proxy");
        static_assert(std::is_same_v<decltype(std::as_const(map)(1)), std::optional<bool>>,
                      "const bool lookup must return optional<bool>");
        static_assert(std::is_assignable<decltype(((*map.begin()).value)), bool>::value,
                      "mutable iteration must expose writable bool proxy");
        static_assert(!std::is_assignable<decltype(((*std::as_const(map).begin()).value)), bool>::value,
                      "const iteration must not write back a bool");
        require(!map(99) && map.empty(), "missing bool read inserted a key");
        map[1] = false;
        auto saved = map(1);
        require(saved.has_value() && !*saved && map.contains(1) && map.rankOf(2) == 1 && map.keyAt(0) == 1,
                "false payload lost membership/rank");
        auto it = map.begin();
        (*it).value = true;
        require(map(1).value() && !*saved, "bool lookup did not copy the current bit");
        for (auto [key, value] : map) { require(key == 1, "bool key differs"); value = false; }
        require(!map(1).value() && map.contains(1), "structured-binding proxy failed");
        auto proxy = map[1]; proxy = true;
        require(map(1).value() && (*std::as_const(map).begin()).value, "copied bool proxy did not write through");
        map.erase(1);
        require(!map(1) && saved.has_value() && !*saved, "optional snapshot changed after erase");
    };
    TreeMap<int, bool> a;
    TreeMapOff<int, bool> b(keys(10));
    checkBool(a); checkBool(b);
    const auto checkDefaults = [](auto& map) {
        map.insert(1, CountedDefault{10});
        int before = CountedDefault::constructed;
        require(map[1].number == 10 && map(1)->number == 10 && CountedDefault::constructed == before,
                "existing access default constructed another value");
        require(!map(2) && map.size() == 1 && CountedDefault::constructed == before,
                "missing lookup inserted or default constructed a value");
        before = CountedDefault::constructed;
        require(!map(3) && CountedDefault::constructed == before, "missing lookup default constructed a value");
        require(map[2].number == 0 && map.size() == 2, "missing subscript failed to initialize value");
    };
    TreeMap<int, CountedDefault> c;
    TreeMapOff<int, CountedDefault> d(keys(10));
    checkDefaults(c); checkDefaults(d);
    const auto checkCopy = [](auto& map) {
        require(map(1).value_or("fallback") == "fallback" && map.empty(), "value_or inserted a missing key");
        map[1] = "original";
        auto saved = std::as_const(map)(1);
        *saved = "copy";
        require(map(1) == "original", "optional writes leaked into the map");
        for (int key = 2; key < 10; ++key)
            map[key] = "grow";
        map.erase(1);
        map.clear();
        require(saved == "copy" && !map(1), "optional copy did not survive growth/erase/clear");
    };
    TreeMap<int, std::string> strings;
    TreeMapOff<int, std::string> stringsOff(keys(10));
    checkCopy(strings); checkCopy(stringsOff);
    const auto checkNested = [](auto& map) {
        map[1] = std::nullopt;
        auto value = map(1);
        require(value.has_value() && !value->has_value() && !map(2), "empty payload confused optional presence");
    };
    TreeMap<int, std::optional<int>> nested;
    TreeMapOff<int, std::optional<int>> nestedOff(keys(10));
    checkNested(nested); checkNested(nestedOff);
    TreeMap<long long, long long> wide;
    wide.insert(1LL << 40, 77); wide.insert(-1, 12);
    static_assert(std::is_same<decltype(wide.keyAt(0)), long long>::value, "wide keyAt must return a value");
    require(wide.keyAt(1) == (1LL << 40) && wide.rankOf(1LL << 40) == 1 && wide(1LL << 40) == 77,
            "wide keys were truncated");
    TreeMap<int, std::array<int, 512>> large;
    for (int key : {4, 2, 6, 1, 3, 5, 7}) {
        std::array<int, 512> value{}; value.fill(key * 10); large.insert(key, value);
    }
    large.erase(4);
    for (auto [key, value] : large) require(value.front() == key * 10 && value.back() == key * 10,
                                           "large payload detached after two-child erase");
}

template<class Map, class Value>
void trivialReuseCase(const std::string& name, Map& map, const Value& filled) {
    testName = name;
    testStep = 0;
    static_assert(std::is_trivial_v<Value>, "this case exercises skipped trivial-value resets");
    map.insert(1, filled);
    require(map.erase(1) && !map.contains(1) && !map(1), "erase exposed an inactive stored value");
    require(map[2] == Value{}, "subscript inherited stale data from a recycled slot");
    map[2] = filled;
    require(map.erase(2) && map[2] == Value{}, "same-key subscript after erase did not default initialize");
    map[2] = filled;
    map[3] = filled;
    map.clear();
    require(map.empty() && !map.contains(2) && !map(2), "clear exposed an inactive stored value");
    require(map[2] == Value{} && map[3] == Value{}, "subscript after clear reused stale trivial values");
    require(map.size() == 2, "default values should remain real entries");
}

template<class Map>
void resourceReleaseCase(const std::string& name, Map& map) {
    testName = name;
    testStep = 0;
    std::array<std::weak_ptr<int>, 8> watched;
    // In the SBT this insertion order gives root 4 two children and successor 5.
    for (int key : {4, 2, 6, 1, 3, 5, 7}) {
        auto resource = std::make_shared<int>(key * 10);
        watched[key] = resource;
        map.insert(key, std::move(resource));
    }
    require(map.erase(4) && watched[4].expired(), "erase did not release the removed key's resource promptly");
    require(!watched[5].expired() && watched[5].use_count() == 1 && **map(5) == 50,
            "two-child erase released or duplicated its successor resource");
    require(map.erase(5) && watched[5].expired(), "second two-child erase retained the removed resource");
    require(!watched[6].expired() && watched[6].use_count() == 1 && **map(6) == 60,
            "successor with a right child lost or duplicated its resource");
    for (auto [key, value] : map) require(value && *value == key * 10, "resource detached from surviving key");
    map.clear();
    for (int key = 1; key <= 7; ++key) require(watched[key].expired(), "clear retained a resource in allocated storage");
    require(!map[4], "shared_ptr subscript after clear did not default initialize");
    auto resource = std::make_shared<int>(99);
    std::weak_ptr<int> reused = resource;
    map[4] = std::move(resource);
    map.clear();
    require(reused.expired(), "clear after slot reuse retained a nontrivial resource");
}

void recycledValueCases() {
    TreeMap<int, int> intMap(2);
    TreeMapOff<int, int> intOff(std::vector<int>{1, 2, 3});
    trivialReuseCase("trivial reset/int/SBT", intMap, 91);
    trivialReuseCase("trivial reset/int/offline", intOff, 91);
    TreeMap<int, bool> boolMap(2);
    TreeMapOff<int, bool> boolOff(std::vector<int>{1, 2, 3});
    trivialReuseCase("trivial reset/bool/SBT", boolMap, true);
    trivialReuseCase("trivial reset/bool/offline", boolOff, true);
    std::array<int, 8> filled;
    filled.fill(91);
    TreeMap<int, std::array<int, 8>> arrayMap(2);
    TreeMapOff<int, std::array<int, 8>> arrayOff(std::vector<int>{1, 2, 3});
    trivialReuseCase("trivial reset/array/SBT", arrayMap, filled);
    trivialReuseCase("trivial reset/array/offline", arrayOff, filled);
    TreeMap<int, std::shared_ptr<int>> resources;
    TreeMapOff<int, std::shared_ptr<int>> resourcesOff(keys(10));
    resourceReleaseCase("resource reset/SBT", resources);
    resourceReleaseCase("resource reset/offline", resourcesOff);
}

template<class Map, class Value>
void observableResetCase(const std::string& name, Map& map, const Value& filled, int& counter) {
    testName = name;
    testStep = 0;
    map.insert(1, filled);
    int before = counter;
    require(map.erase(1) && counter == before + 1, "single-element erase skipped or duplicated observable reset");
    map.insert(1, filled);
    before = counter;
    map.clear();
    require(map.empty() && counter > before, "clear skipped observable resets of allocated value slots");
    require(map[1].number == 0, "subscript after observable clear did not default initialize");
}

void observableResetCases() {
    TreeMap<int, CountedDefault> counted;
    TreeMapOff<int, CountedDefault> countedOff(std::vector<int>{1});
    observableResetCase("default construction reset/SBT", counted, CountedDefault{91}, CountedDefault::constructed);
    observableResetCase("default construction reset/offline", countedOff, CountedDefault{91}, CountedDefault::constructed);
    TrivialTemplateAssignment filled{};
    filled.number = 91;
    TreeMap<int, TrivialTemplateAssignment> assigned;
    TreeMapOff<int, TrivialTemplateAssignment> assignedOff(std::vector<int>{1});
    observableResetCase("template assignment reset/SBT", assigned, filled, TrivialTemplateAssignment::assigned);
    observableResetCase("template assignment reset/offline", assignedOff, filled, TrivialTemplateAssignment::assigned);
}

struct SharedCompare {
    std::shared_ptr<bool> descending = std::make_shared<bool>(true);
    bool operator()(int a, int b) const {
        require(bool(descending), "moved-from comparator lost its reusable state");
        return *descending ? a > b : a < b;
    }
};
template<bool Offline, class Map>
void copyMoveCase(Map& original, SharedCompare compare) {
    std::map<int, int, SharedCompare> reference(compare);
    for (int key : {4, 2, 6, 1, 3, 5, 7}) { original.insert(key, key * 10); reference.emplace(key, key * 10); }
    original.erase(2); reference.erase(2); original.erase(6); reference.erase(6);
    Map copy(original);
    copy[1] = 100; copy.insert(8, 80);
    snapshot(original, reference);
    copy = original;
    snapshot(copy, reference);
    Map moved(std::move(copy));
    snapshot(moved, reference);
    copy.clear();
    if constexpr (!Offline) {
        copy.reserve(2); copy.insert(8, 80); copy.insert(9, 90);
        require(copy.keyAt(0) == 9, "moved SBT source reuse differs");
    }
    copy = original; copy.insert(2, 20);
    require(copy.contains(2) && !original.contains(2), "copied container does not own independent storage");
    Map assigned(original); assigned = std::move(moved);
    snapshot(assigned, reference);
    moved.clear(); moved = original;
    snapshot(moved, reference);
    original.clear();
    snapshot(assigned, reference);
}
void copyMoveCases() {
    testName = "copy/move with recycled slots";
    SharedCompare compare;
    TreeMap<int, int, SharedCompare> a(compare);
    TreeMapOff<int, int, SharedCompare> b(keys(10), compare);
    TreeMap<int, int, SharedCompare> c(16, compare);
    copyMoveCase<false>(a, compare); copyMoveCase<true>(b, compare); copyMoveCase<false>(c, compare);
}

void moveOnlyContainerCases() {
    testName = "move-only map vector growth";
    using Map = TreeMap<int, std::unique_ptr<int>>;
    using Off = TreeMapOff<int, std::unique_ptr<int>>;
    static_assert(std::is_nothrow_move_constructible<Map>::value, "map vector growth must move noncopyable payloads");
    static_assert(std::is_nothrow_move_constructible<Off>::value, "offline map vector growth must move noncopyable payloads");
    std::vector<Map> maps;
    std::vector<Off> offlineMaps;
    int reallocations = 0;
    for (int i = 0; i < 48; ++i) {
        auto oldCapacity = maps.capacity();
        maps.emplace_back(); offlineMaps.emplace_back(std::vector<int>{i, i + 1000});
        if (maps.capacity() != oldCapacity) ++reallocations;
        maps.back().insert(i, std::make_unique<int>(i * 10));
        maps.back().insert(i + 1000, std::make_unique<int>(i * 10 + 1));
        offlineMaps.back().insert(i, std::make_unique<int>(i * 10));
        offlineMaps.back().insert(i + 1000, std::make_unique<int>(i * 10 + 1));
        for (int j = 0; j <= i; ++j) {
            require(maps[j].size() == 2 && *maps[j][j] == j * 10 && *maps[j][j + 1000] == j * 10 + 1,
                    "SBT vector reallocation lost payload");
            require(offlineMaps[j].size() == 2 && *offlineMaps[j][j] == j * 10 &&
                    *offlineMaps[j][j + 1000] == j * 10 + 1, "offline vector reallocation lost payload");
        }
    }
    require(reallocations >= 4, "vector test did not exercise repeated growth");
    Map source; source.insert(7, std::make_unique<int>(70));
    Map moved(std::move(source));
    require(source.empty() && source.begin() == source.end(), "moved SBT source is not empty");
    source.reserve(2); source.insert(9, std::make_unique<int>(90));
    require(*source[9] == 90 && *moved[7] == 70, "moved SBT source reuse differs");
    Off offline(std::vector<int>{1, 3}); offline.insert(1, std::make_unique<int>(10));
    Off movedOffline(std::move(offline));
    require(offline.empty() && !offline.contains(1), "moved offline source is not empty");
    expectThrow<std::out_of_range>([&] { offline.insert(99, std::make_unique<int>(990)); });
    expectThrow<std::out_of_range>([&] { movedOffline.insert(99, std::make_unique<int>(990)); });
    offline = Off(std::vector<int>{99, 101}); offline.insert(99, std::make_unique<int>(990));
    require(*offline[99] == 990 && *movedOffline[1] == 10, "offline reassignment did not restore keys");
    movedOffline = std::move(offline);
    require(*movedOffline[99] == 990 && offline.empty(), "offline move assignment failed");
    expectThrow<std::out_of_range>([&] { offline[101] = std::make_unique<int>(1010); });
    offline = Off(std::vector<int>{101}); offline[101] = std::make_unique<int>(1010);
    require(*offline[101] == 1010, "reassigned offline subscript failed");
}
template<class Key, class Compare>
void radixCase(Compare compare) {
    std::mt19937_64 random(2313);
    for (int n : {0, 1, 1023, 1024, 4097}) {
        std::vector<Key> input(n);
        for (Key& key : input)
            key = static_cast<Key>(random());
        if (n)
            input[0] = std::numeric_limits<Key>::min();
        if (n > 1)
            input[1] = std::numeric_limits<Key>::max();
        auto expected = input;
        std::sort(expected.begin(), expected.end(), compare);
        expected.erase(std::unique(expected.begin(), expected.end()), expected.end());
        for (int order = 0; order < 3; ++order) {
            TreeMapOff<Key, bool, Compare> map(input, compare);
            require(map.empty(), "candidate registration inserted elements");
            for (Key key : input)
                map.insert(key, true);
            require(map.size() == static_cast<int>(expected.size()), "radix distinct count");
            for (int i = 0; i < map.size(); ++i) {
                require(map.keyAt(i) == expected[i], "radix ordering");
                require(map.rankOf(expected[i]) == i && map(expected[i]), "radix lookup");
            }
            std::sort(input.begin(), input.end(), compare);
            if (order == 1)
                std::reverse(input.begin(), input.end());
        }
    }
}

template<class Key, class Compare>
void candidateSortCase(Compare compare) {
    std::mt19937_64 random(2401);
    for (int n : {0, 1, 2, 63, 64, 65, 128, 256, 1024, 4097, 8193}) {
        for (int kind = 0; kind < 4; ++kind) {
            std::vector<Key> input(n);
            for (int i = 0; i < n; ++i) {
                auto value = random();
                if (kind == 0)
                    input[i] = static_cast<Key>(value);
                else if (kind == 1)
                    input[i] = static_cast<Key>(int(value % 65536) - 32768);
                else if (kind == 2)
                    input[i] = static_cast<Key>(value % 7);
                else
                    input[i] = static_cast<Key>((value % 1024) << 11);
            }
            if (n && kind == 0)
                input[0] = std::numeric_limits<Key>::min();
            if (n > 1 && kind == 0)
                input[1] = std::numeric_limits<Key>::max();
            auto expected = input;
            std::sort(expected.begin(), expected.end(), compare);
            for (int order = 0; order < 3; ++order) {
                input = std::move(input) | seq::sorted(compare);
                require(input == expected, "candidate sorting lost values or order");
                if (order == 1)
                    std::reverse(input.begin(), input.end());
            }
        }
    }
}

template<class Key>
void radixTypeCases() {
    radixCase<Key>(std::less<Key>{});
    radixCase<Key>(std::greater<Key>{});
    candidateSortCase<Key>(std::less<>{});
    candidateSortCase<Key>(std::greater<Key>{});
}

void radixCases() {
    testName = "radix sorting";
    radixTypeCases<signed char>();
    radixTypeCases<unsigned char>();
    radixTypeCases<short>();
    radixTypeCases<unsigned short>();
    radixTypeCases<int>();
    radixTypeCases<unsigned int>();
    radixTypeCases<long long>();
    radixTypeCases<unsigned long long>();
    radixCase<int>(std::less<>{});
    candidateSortCase<int>(std::less<>{});
    candidateSortCase<long long>(std::greater<>{});
    candidateSortCase<bool>(std::less<bool>{});
    candidateSortCase<int>([](int a, int b) { return a > b; });
    // Equal upper digits must be skipped without changing the active buffer.
    for (int shift : {0, 8}) {
        std::vector<long long> narrow(4097);
        for (int i = 0; i < static_cast<int>(narrow.size()); ++i)
            narrow[i] = (256LL + i % 7) << shift;
        TreeMapOff<long long, int> map(narrow);
        for (auto key : narrow)
            map[key] = static_cast<int>(key);
        require(map.size() == 7, "radix duplicate compaction");
        for (int i = 0; i < 7; ++i)
            require(map.keyAt(i) == ((256LL + i) << shift), "radix skipped digit ordering");
    }
    TreeMapOff<std::string, int> strings({"z", "a", "z", "b"});
    strings["a"] = 1;
    strings["z"] = 2;
    require(strings.keyAt(0) == "a" && strings.keyAt(1) == "z", "generic sort fallback");
}

} // namespace

int main() {
    try {
        differentialCases();
        radixCases();
        capacityCases();
        valueCases();
        recycledValueCases();
        observableResetCases();
        copyMoveCases();
        moveOnlyContainerCases();
        std::cout << "PASS: TreeMap/TreeMapOff C++17 differential and regression checks\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
