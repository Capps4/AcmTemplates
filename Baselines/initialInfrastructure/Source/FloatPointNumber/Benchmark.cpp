#include <bits/stdc++.h>
#include "BenchmarkBaseline.hpp"
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
template <class F>
std::uint64_t arithmetic(const std::vector<double>& values) {
    F sum = 0;
    for (double number : values) {
        F value(number);
        sum += value * value / (value + F(2));
    }
    return std::uint64_t(sum.val() * 1000);
}
template <class F>
std::uint64_t parse(const std::string& text) {
    std::istringstream input(text);
    F value; double sum = 0;
    while (input >> value) sum += value.val();
    return std::uint64_t(sum * 1000);
}
template <class F>
std::uint64_t format(const std::vector<double>& values) {
    std::ostringstream output;
    std::ostream& writer = output;
    for (double value : values) writer << F(value) << ' ';
    auto text = output.str();
    std::uint64_t sum = 0;
    for (unsigned char c : text) sum += c;
    return sum;
}
int main() {
    std::vector<double> values(1000000);
    std::mt19937 rng(20261001);
    for (double& value : values) value = 1 + rng() % 1000 / 7.;
    compare("arithmetic-1M", [&] { return arithmetic<Legacy::Float>(values); }, [&] { return arithmetic<Float>(values); });
    values.resize(30000);
    std::ostringstream data;
    for (double value : values) data << std::setprecision(17) << value << ' ';
    auto text = data.str();
    compare("parse-30K", [&] { return parse<Legacy::Float>(text); }, [&] { return parse<Float>(text); });
    compare("format-30K", [&] { return format<Legacy::Float>(values); }, [&] { return format<Float>(values); });
}
