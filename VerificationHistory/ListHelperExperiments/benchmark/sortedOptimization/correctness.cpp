#include "Candidates.hpp"
#include "../../Sorted.hpp"
#include <array>
#include <cstring>
#include <deque>
#include <memory>
#include <memory_resource>
#include <random>
#include <stdexcept>

std::size_t checks = 0;
void require(bool condition) {
    ++checks;
    if (!condition)
        throw std::runtime_error("sorted correctness mismatch at check " + std::to_string(checks));
}

template<int B, bool Skip, class T, class Compare>
void checkOne(const std::vector<T>& input, Compare compare) {
    auto expected = input;
    std::sort(expected.begin(), expected.end(), compare);
    auto original = input;
    require((input | candidates::sorted<B, Skip>(compare)) == expected);
    require(input == original);
    auto moved = input;
    require((std::move(moved) | candidates::sorted<B, Skip>(compare)) == expected);
}

template<class T, class Compare>
void checkSort(const std::vector<T>& input, Compare compare) {
    checkOne<8, false>(input, compare);
    checkOne<8, true>(input, compare);
    checkOne<11, true>(input, compare);
    checkOne<15, true>(input, compare);
    auto expected = input;
    std::sort(expected.begin(), expected.end(), compare);
    require((input | seq::sorted(compare)) == expected);
    auto moved = input;
    require((std::move(moved) | seq::sorted(compare)) == expected);
}

template<class T>
void integerCases() {
    using U = std::make_unsigned_t<T>;
    std::mt19937_64 random(5551);
    for (int n : {0, 1, 2, 31, 255, 256, 257, 511, 1024, 2049, 4095, 4096, 4097}) {
        for (int kind = 0; kind < 5; ++kind) {
            std::vector<T> input(n);
            for (auto& x : input) {
                U bits = U(random());
                if (kind == 1)
                    bits = U(random() % 7);
                if (kind == 2)
                    bits = U(U(random() % 7) << (sizeof(T) > 1 ? 8 : 0));
                if (kind == 3)
                    bits = 0;
                std::memcpy(&x, &bits, sizeof(T));
            }
            if (n && kind == 0)
                input[0] = std::numeric_limits<T>::min();
            if (n > 1 && kind == 0)
                input[1] = std::numeric_limits<T>::max();
            if (kind == 4)
                std::sort(input.rbegin(), input.rend());
            checkSort(input, std::less<>{});
            checkSort(input, std::less<T>{});
            checkSort(input, std::greater<>{});
            checkSort(input, std::greater<T>{});
        }
    }
}

void containerCases() {
    auto comparator = [state = std::make_unique<int>(0)](int a, int b) mutable {
        ++*state;
        return a > b;
    };
    require((std::vector<int>{3, 1, 2} | seq::sorted(std::move(comparator))) ==
            std::vector<int>({3, 2, 1}));
    std::vector<bool> flags{true, false, true};
    require((flags | seq::sorted()) == std::vector<bool>({false, true, true}));
    require((std::string("cab") | seq::sorted()) == "abc");
    std::array<int, 4> array{3, 1, 2, 1};
    require((array | seq::sorted()) == std::array<int, 4>{1, 1, 2, 3});
    require((std::deque<int>{3, 1, 2} | seq::sorted()) == std::deque<int>({1, 2, 3}));
    std::vector<std::unique_ptr<int>> pointers;
    for (int value : {3, 1, 2})
        pointers.push_back(std::make_unique<int>(value));
    auto result = std::move(pointers) | seq::sorted(
        [](const auto& a, const auto& b) { return *a < *b; });
    require(*result.front() == 1 && *result.back() == 3);
    std::pmr::monotonic_buffer_resource resource;
    std::pmr::vector<int> allocated(&resource);
    for (int i = 5000; i; --i)
        allocated.push_back((i * 719 % 5000) * 512);
    auto output = std::move(allocated) | seq::sorted(std::greater<int>{});
    require(output.get_allocator().resource() == &resource);
    require(std::is_sorted(output.begin(), output.end(), std::greater<>{}));
    // An already-sorted moved vector preserves the original allocation.
    auto address = output.data();
    auto movedAgain = std::move(output) | seq::sorted(std::greater<int>{});
    require(movedAgain.data() == address);
}

int main() {
    integerCases<std::int8_t>();
    integerCases<std::uint8_t>();
    integerCases<std::int16_t>();
    integerCases<std::uint16_t>();
    integerCases<std::int32_t>();
    integerCases<std::uint32_t>();
    integerCases<std::int64_t>();
    integerCases<std::uint64_t>();
    containerCases();
    std::cout << "PASS: " << checks << " candidate sorted checks\n";
}
