#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
inline std::mt19937_64
    rng(static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()));
