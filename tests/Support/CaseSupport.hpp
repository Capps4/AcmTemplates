#pragma once
#include "TestSupport.hpp"
#include <chrono>
#include <exception>
#include <string>
#include <string_view>

inline bool selectedCase(std::string_view id) {
    const char *selected = std::getenv("TEST_CASE");
    if (not selected or not *selected)
        return true;
    std::string_view name(selected);
    return id == name or (id.size() > name.size() and
           id.substr(id.size() - name.size() - 1) == std::string("/") + std::string(name));
}

template <class F>
void runCase(const char *id, F body) {
    if (not selectedCase(id))
        return;
    // Stable per-case replay: selecting a case does not depend on previous RNG draws.
    std::uint64_t mixed = testSeed;
    for (unsigned char c : std::string_view(id)) mixed = (mixed ^ c) * 0x100000001b3ULL;
    testRng.seed(mixed);
    test_context::id = id;
    test_context::step = -1;
    test_context::input.clear();
    std::cout << "CASE_BEGIN " << id << std::endl;
    auto begin = std::chrono::steady_clock::now();
    try {
        body();
    } catch (const std::exception &error) {
        std::cerr << "CASE_FAIL " << id << ": " << error.what()
                  << "; TEST_SEED=" << testSeed << std::endl;
        std::exit(1);
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "CASE_PASS " << id << '\n' << "CASE_TIME " << id << ' '
              << std::chrono::duration<double, std::milli>(end - begin).count() << std::endl;
}

// Expensive conditioning witnesses remain available explicitly, outside the default budget.
template <class F>
void runStressCase(const char *id, F body) {
    const char *selected = std::getenv("TEST_CASE");
    if (selected and *selected) runCase(id, body);
}
