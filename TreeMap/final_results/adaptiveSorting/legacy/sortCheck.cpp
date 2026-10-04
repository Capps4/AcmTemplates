#include "sort.hpp"
#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>

using namespace std;

void require(bool condition, const char* message) {
    if (!condition)
        throw runtime_error(message);
}

template<class T, class Compare>
void integerCases(Compare compare) {
    mt19937_64 random(2401);
    for (int n : {0, 1, 2, 63, 64, 65, 128, 256, 1024, 4097, 8193}) {
        for (int kind = 0; kind < 4; ++kind) {
            vector<T> input(n);
            for (int i = 0; i < n; ++i) {
                auto value = random();
                if (kind == 0)
                    input[i] = static_cast<T>(value);
                if (kind == 1)
                    input[i] = static_cast<T>(int(value % 65536) - 32768);
                if (kind == 2)
                    input[i] = static_cast<T>(value % 7);
                if (kind == 3)
                    input[i] = static_cast<T>((value % 1024) << 11);
            }
            if (n && kind == 0)
                input[0] = numeric_limits<T>::min();
            if (n > 1 && kind == 0)
                input[1] = numeric_limits<T>::max();
            auto expected = input;
            std::sort(expected.begin(), expected.end(), compare);
            for (int order = 0; order < 3; ++order) {
                auto original = input;
                auto copy = sorted(input, compare);
                require(copy == expected && input == original, "sorted modified input or returned wrong data");
                auto address = input.data();
                sort(input, compare);
                require(input == expected && input.data() == address, "in-place sort changed data/storage");
                if (order == 1)
                    reverse(input.begin(), input.end());
            }
        }
    }
}

template<class T>
void typeCases() {
    integerCases<T>(less<T>{});
    integerCases<T>(greater<T>{});
}

template<int B, class T>
void radixCases() {
    mt19937_64 random(891);
    vector<T> input(3079);
    for (auto& value : input)
        value = static_cast<T>(random());
    input[0] = numeric_limits<T>::min();
    input[1] = numeric_limits<T>::max();
    auto expected = input;
    std::sort(expected.begin(), expected.end());
    auto bounds = minmax_element(input.begin(), input.end());
    using Unsigned = make_unsigned_t<T>;
    Unsigned minValue = static_cast<Unsigned>(*bounds.first);
    Unsigned maxV = static_cast<Unsigned>(*bounds.second) - minValue;
    auto descending = input;
    sorting::radixSort<B, false>(input.begin(), input.end(), minValue, maxV);
    require(input == expected, "radix ascending failed");
    sorting::radixSort<B, true>(descending.begin(), descending.end(), minValue, maxV);
    reverse(expected.begin(), expected.end());
    require(descending == expected, "radix descending failed");
}

void containerCases() {
    const vector<int> values{3, 1, 2, 1};
    require(sorted(values) == vector<int>({1, 1, 2, 3}), "const input copy");
    require(sorted(vector<int>{3, 1, 2}) == vector<int>({1, 2, 3}), "rvalue input");
    array<int, 4> a{3, 1, 2, 1};
    require(sorted(a) == array<int, 4>{1, 1, 2, 3}, "array copy");
    sort(a);
    require(a == array<int, 4>{1, 1, 2, 3}, "array sort");
    deque<int> d{3, 1, 2};
    sort(d, greater<>{});
    require(d == deque<int>({3, 2, 1}), "deque sort");
    vector<bool> bits{true, false, true, false};
    sort(bits);
    require(bits == vector<bool>({false, false, true, true}), "native bool proxies");
    require(sorted(string("cab")) == "abc", "string sort");
    list<int> linked{3, 1, 2};
    require(sorted(linked) == list<int>({1, 2, 3}) && linked.front() == 3, "list copy");
    forward_list<int> forward{3, 1, 2};
    sort(forward);
    require(forward == forward_list<int>({1, 2, 3}), "forward_list sort");
    // The container overload must not conflict with std::sort(iterator, iterator).
    auto first = a.begin(), last = a.end();
    sort(first, last);
    sort(first, last, greater<int>{});
    require(a.front() == 3, "std::sort overload resolution");
    vector<string> words{"z", "aa", "b"};
    sort(words, [](const string& x, const string& y) { return x > y; });
    require(words == vector<string>({"z", "b", "aa"}), "custom comparator fallback");
    vector<unique_ptr<int>> owners;
    for (int value : {3, 1, 2})
        owners.push_back(make_unique<int>(value));
    auto moved = sorted(std::move(owners), [](const auto& x, const auto& y) { return *x < *y; });
    require(*moved[0] == 1 && *moved[2] == 3, "move-only payloads");
    deque<long long> large;
    for (int i = 8193; i; --i)
        large.push_back((i * 733LL) % 2048 - 1024);
    auto expected = large;
    std::sort(expected.begin(), expected.end());
    sort(large);
    require(large == expected, "deque radix path");
}

int main() {
    try {
        typeCases<signed char>();
        typeCases<unsigned char>();
        typeCases<short>();
        typeCases<unsigned short>();
        typeCases<int>();
        typeCases<unsigned int>();
        typeCases<long long>();
        typeCases<unsigned long long>();
        integerCases<int>(less<>{});
        integerCases<long long>(greater<>{});
        radixCases<4, signed char>();
        radixCases<8, short>();
        radixCases<11, short>();
        radixCases<16, short>();
        radixCases<11, long long>();
        radixCases<16, unsigned long long>();
        containerCases();
        cout << "PASS: adaptive sort/sorted C++17 regression checks\n";
    } catch (const exception& error) {
        cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
