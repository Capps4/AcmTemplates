#include <bits/stdc++.h>
namespace Legacy {
#include "Original.hpp"
}
#undef cin
#undef cout
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
template <class Input>
std::uint64_t read(FILE* file) {
    std::rewind(file); std::clearerr(file);
    auto input = std::make_unique<Input>(file);
    std::uint64_t sum = 0; long long value;
    while (*input >> value) sum += std::uint64_t(value);
    return sum;
}
template <class Output>
std::uint64_t write(FILE* file, const std::vector<long long>& values) {
    std::rewind(file); std::clearerr(file);
    auto output = std::make_unique<Output>(file);
    for (auto value : values) *output << value << '\n';
    output->flush();
    if (!*output) std::exit(1);
    // A full byte checksum is also made outside the timed loops below.
    return std::uint64_t(std::ftell(file));
}
std::uint64_t bytes(FILE* file) {
    std::rewind(file); std::uint64_t sum = 0; char block[4096];
    while (auto count = std::fread(block, 1, sizeof(block), file))
        for (std::size_t i = 0; i < count; ++i) sum = sum * 131 + static_cast<unsigned char>(block[i]);
    return sum;
}
int main() {
    std::mt19937_64 rng(20261001);
    for (bool wide : {false, true}) {
        std::vector<long long> values(1000000);
        std::ostringstream text;
        for (auto& value : values) { value = wide ? static_cast<long long>(rng()) : int(rng() % 2000001) - 1000000; text << value << '\n'; }
        auto data = text.str();
        FILE* file = std::tmpfile();
        std::fwrite(data.data(), 1, data.size(), file);
        compare(wide ? "read-i64-1M" : "read-small-1M", [&] { return read<Legacy::Qinput>(file); }, [&] { return read<QInput>(file); });
        std::fclose(file);
        FILE* before = std::tmpfile(); FILE* after = std::tmpfile();
        write<Legacy::Qoutput>(before, values); write<QOutput>(after, values);
        if (bytes(before) != bytes(after)) std::exit(1);
        compare(wide ? "write-i64-1M" : "write-small-1M", [&] { return write<Legacy::Qoutput>(before, values); }, [&] { return write<QOutput>(after, values); });
        std::fclose(before); std::fclose(after);
    }
}
