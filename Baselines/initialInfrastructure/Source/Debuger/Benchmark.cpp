#include <bits/stdc++.h>
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
int main() {
    auto scan = [](const char* options) {
        std::uint64_t sum = 0;
        for (int repeat = 0; repeat < 100000; ++repeat) {
            benchmarkClobber();
            for (auto p = options; *p; ++p) sum += static_cast<unsigned char>(*p);
        }
        return sum;
    };
    compare("options-scan-100K", [&] { return scan("color: false, space: false, precision: 6"); },
            [&] { return scan(debugerOptions); });
    // Local bits/stdc++.h maps strdup to the actual debugger registration hook.
}
