#include "Input.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    auto s = workload(input);
    return measure(input, "runs: random 4-letter / Fibonacci word", [&] {
        return checksum(runEnumeration(s), s);
    });
}
