#include "Final.hpp"
#include "../TestSupport.hpp"
#include <cstdint>
#include <sstream>
#include <string>
#include <thread>
std::string formatFromOtherTranslationUnit(double value);
static_assert((Float(2) + Float(3) * Float(4)).val() == 14);
static_assert((Float(9) / Float(2)).val() == 4.5);
static_assert((2 + Float(3)).val() == 5 && (Float(3) * 2.0).val() == 6);
static_assert(Float(0) == 5E-13 && !(Float(0) == 2E-12));
static_assert(Float(0) < 5E-13 && Float(0) >= 5E-13);
static_assert(std::is_same_v<decltype(sqrt(Float(4))), Float>);
static_assert(std::is_same_v<decltype(std::sqrt(Float(4))), double>);
struct RealOutput {
    double value = 0; int digits = 0;
    RealOutput& writeReal(double number, int precision) { value = number; digits = precision; return *this; }
};
int main() {
    for (int i = 0; i < 100000; ++i) {
        double a = randomInt(-100000, 100000) / 37.0;
        double b = randomInt(1, 100000) / 31.0;
        Float x(a), y(b);
        CHECK((x + y).val() == a + b && (x - y).val() == a - b);
        CHECK((x * y).val() == a * b);
        CHECK((x / y).val() == double(static_cast<long double>(a) / static_cast<long double>(b)));
        CHECK((x + 2).val() == a + 2 && (2 - x).val() == 2 - a);
        CHECK(x.round<int>() == int(std::round(a)));
        CHECK(std::abs(sin(x).val() - std::sin(a)) < 1E-12);
        CHECK(std::abs(cos(x).val() - std::cos(a)) < 1E-12);
        CHECK(std::abs(atan2(x, y).val() - std::atan2(a, b)) < 1E-12);
        CHECK(abs(x).val() == std::abs(a));
    }
    CHECK(Float(-0.6).round<int>() == -1 && Float(-1.5).round<int>() == -2);
    CHECK(Float(0.5).round<int>() == 1 && Float(-0.49).round<int>() == 0);
    CHECK(Float(double(std::numeric_limits<long long>::min())).round<long long>() == std::numeric_limits<long long>::min());
    Float nan(std::numeric_limits<double>::quiet_NaN());
    CHECK(nan != nan && !(nan == 0) && !(nan <= 0));
    Float inf(std::numeric_limits<double>::infinity());
    CHECK(inf == inf);
    CHECK(std::abs(Float::PI.val() - std::acos(-1.)) < 1E-15);
    std::istringstream input("-1.25 4.5e2 malformed");
    Float a, b, unchanged(17);
    input >> a >> b >> unchanged;
    CHECK(a.val() == -1.25 && b.val() == 450 && input.fail() && unchanged.val() == 17);
    input >> unchanged;
    CHECK(unchanged.val() == 17);
    Float::setprecision(3);
    std::ostringstream output;
    output << a;
    CHECK(output.str() == "-1.250" && formatFromOtherTranslationUnit(2.5) == "2.500");
    RealOutput adapter;
    adapter << a;
    CHECK(adapter.value == -1.25 && adapter.digits == 3);
    Float::setprecision(6);
    FloatPointNumber<long double> extended(1.25L);
    std::stringstream extendedStream;
    extendedStream << extended;
    FloatPointNumber<long double> parsed;
    extendedStream >> parsed;
    CHECK(parsed.val() == 1.25L);
    std::vector<std::thread> threads;
    for (int worker = 0; worker < 4; ++worker) threads.emplace_back([worker] {
        for (int i = 0; i < 1000; ++i) {
            double expected = 123456789.125 + worker + i;
            std::ostringstream text; text << std::fixed << std::setprecision(3) << expected;
            std::istringstream input(text.str());
            Float parsed; input >> parsed;
            CHECK(input && parsed.val() == expected);
        }
    });
    for (auto& thread : threads) thread.join();
    std::cout << "FloatPointNumber constexpr/mixed scalar, 100K raw-math oracle, signed rounding, stream errors, parallel parsing, two-TU precision PASS\n";
}
