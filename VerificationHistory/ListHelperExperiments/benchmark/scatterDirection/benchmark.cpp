#include "backward.hpp"
#include "forward.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <memory>
#include <random>
#include <stdexcept>

using Clock = std::chrono::steady_clock;

template<bool Forward, int B, bool Descending, class List>
__attribute__((noinline, aligned(64)))
void run(List& a, typename List::value_type lo, typename List::value_type hi) {
    if constexpr (B) {
        if constexpr (Forward)
            forward::radixSort<B, Descending>(a, lo, hi);
        else
            backward::radixSort<B, Descending>(a, lo, hi);
    } else {
        using Compare = std::conditional_t<Descending, std::greater<>, std::less<>>;
        if constexpr (Forward)
            a = std::move(a) | forward::sorted(Compare{});
        else
            a = std::move(a) | backward::sorted(Compare{});
    }
}

template<int B, bool Descending, class List>
void measure(const List& input, const char* container, const char* type,
             const char* distribution, std::mt19937_64& random, int rounds) {
    using T = typename List::value_type;
    using Compare = std::conditional_t<Descending, std::greater<>, std::less<>>;
    auto expected = std::make_unique<List>(input);
    std::sort(expected->begin(), expected->end(), Compare{});
    auto [lo, hi] = std::minmax_element(input.begin(), input.end());
    using Fn = void (*)(List&, T, T);
    std::array<std::pair<int, Fn>, 2> methods{{
        {0, run<false, B, Descending, List>},
        {1, run<true, B, Descending, List>}
    }};
    for (int round = -1; round < rounds; ++round) {
        std::shuffle(methods.begin(), methods.end(), random);
        for (auto [method, function] : methods) {
            double total = 0;
            int repeats = 0;
            do {
                auto a = std::make_unique<List>(input);
                std::atomic_signal_fence(std::memory_order_seq_cst);
                auto start = Clock::now();
                function(*a, *lo, *hi);
                auto stop = Clock::now();
                std::atomic_signal_fence(std::memory_order_seq_cst);
                if (*a != *expected)
                    throw std::runtime_error("output mismatch");
                total += std::chrono::duration<double, std::nano>(stop - start).count();
                ++repeats;
            } while (total < 500000 && repeats < 512);
            if (round >= 0)
                std::cout << (B ? "kernel" : "sorted") << ',' << container << ','
                          << type << ',' << input.size() << ',' << distribution << ','
                          << (Descending ? "desc" : "asc") << ',' << B << ','
                          << method << ',' << round << ',' << repeats << ','
                          << total / repeats << '\n';
        }
    }
}

template<class List>
void fill(List& a, int kind, std::mt19937_64& random) {
    using T = typename List::value_type;
    using U = std::make_unsigned_t<T>;
    for (auto& x : a) {
        U bits = U(random());
        if (kind == 1)
            bits = U(random() % (1 << 22));
        if (kind == 2)
            bits = U(random() % 256) << 16;
        if (kind == 3)
            bits = U(random() % 16) - U(8);
        if (kind == 7)
            bits = 177;
        std::memcpy(&x, &bits, sizeof(T));
    }
    if (kind == 0 && a.size() > 1) {
        a[0] = std::numeric_limits<T>::min();
        a[1] = std::numeric_limits<T>::max();
    }
    if (kind >= 4 && kind <= 6) {
        std::sort(a.begin(), a.end());
        if (kind == 5)
            std::reverse(a.begin(), a.end());
        if (kind == 6)
            for (std::size_t j = 0; j < a.size() / 64 + 1; ++j)
                std::swap(a[random() % a.size()], a[random() % a.size()]);
    }
}

template<class T>
void sweep(const char* type, std::mt19937_64& random, int rounds) {
    const char* names[] = {"full", "bits22", "gaps", "few", "ordered",
                           "reverse", "nearly", "equal"};
    for (int n : {128, 512, 4096, 65536, 1000000}) {
        for (int kind = 0; kind < 8; ++kind) {
            std::vector<T> a(n);
            fill(a, kind, random);
            measure<0, false>(a, "vector", type, names[kind], random, rounds);
            measure<0, true>(a, "vector", type, names[kind], random, rounds);
            if (n == 1000000 && kind <= 3) {
                measure<8, false>(a, "vector", type, names[kind], random, rounds);
                measure<8, true>(a, "vector", type, names[kind], random, rounds);
                measure<11, false>(a, "vector", type, names[kind], random, rounds);
                measure<11, true>(a, "vector", type, names[kind], random, rounds);
                measure<15, false>(a, "vector", type, names[kind], random, rounds);
                measure<15, true>(a, "vector", type, names[kind], random, rounds);
            }
        }
    }
    for (int kind = 0; kind < 8; ++kind) {
        auto a = std::make_unique<std::array<T, 65536>>();
        fill(*a, kind, random);
        measure<0, false>(*a, "array", type, names[kind], random, rounds);
        measure<0, true>(*a, "array", type, names[kind], random, rounds);
    }
}

int main(int argc, char** argv) {
    auto seed = argc > 1 ? std::stoull(argv[1]) : 177613;
    int rounds = argc > 2 ? std::stoi(argv[2]) : 7;
    std::mt19937_64 random(seed);
    std::cout << "stage,container,type,n,distribution,direction,bits,method,round,repeats,ns\n"
              << std::fixed << std::setprecision(3);
    sweep<std::int32_t>("i32", random, rounds);
    sweep<std::int64_t>("i64", random, rounds);
    std::cerr << "PASS: every timed output equals std::sort; seed=" << seed << '\n';
}
