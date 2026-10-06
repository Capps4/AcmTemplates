#pragma once
#include "TestSupport.hpp"
#include <exception>
#include <string>

inline int addedCaseCount = 0;

template <class F>
void runCase(const char *id, F body) {
    std::cout << "CASE_BEGIN " << id << std::endl;
    try {
        body();
    } catch (const std::exception &error) {
        std::cerr << "CASE_FAIL " << id << ": " << error.what() << std::endl;
        std::exit(1);
    }
    ++addedCaseCount;
    std::cout << "CASE_PASS " << id << std::endl;
}

inline int finishCases(int expected) {
    CHECK(addedCaseCount == expected);
    std::cout << "ADDED_CASES " << addedCaseCount << std::endl;
    return 0;
}
