#include "Workload.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    auto [a, c] = workload(input);
    return measure(input, "polynomial reduction: dense / sparse recurrence at 1e18", [&] {
        return std::uint64_t(linearRecurrence(a, c, recurrenceIndex).val());
    });
}
