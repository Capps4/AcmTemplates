#include "Final.hpp"
std::mt19937_64* engineFromOtherTranslationUnit() { return &rng; }
std::uint64_t drawFromOtherTranslationUnit() { return rng(); }
