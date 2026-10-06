#include "../../../../src/Clarketech/FastInputOutput/code.hpp"
#undef cin
#undef cout
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    FILE *f = std::tmpfile();
    if (!f)
        return 2;
    std::string text;
    for (int i = 0; i < n; ++i)
        text += std::to_string(input.shape ? -i : i) + " ";
    std::fwrite(text.data(), 1, text.size(), f);
    std::fflush(f);

    int result =
        measure(input, "integer buffered input including file reads", [&]() -> std::uint64_t {
            std::rewind(f);
            auto in = std::make_unique<Qinput>(f);
            std::uint64_t sum = 0;
            for (int i = 0; i < n; ++i) {
                long long x = 0;
                *in >> x;
                sum += std::uint64_t(x);
            }
            return sum;
        });
    std::fclose(f);
    return result;
}
