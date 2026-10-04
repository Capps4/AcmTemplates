#include "Final.hpp"
#include "../TestSupport.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

std::string formatFromOtherTranslationUnit(double value);
constexpr double eps = 1E-12;

static_assert(std::is_same_v<Float, FloatPointNumber<double>>);
static_assert(std::is_same_v<decltype(Float().val()), double>);
static_assert(std::is_convertible_v<double, Float> && std::is_convertible_v<int, Float>);
static_assert(!std::is_convertible_v<Float, double>);
static_assert(Float().val() == 0);
static_assert((Float(2) + Float(3) * Float(4)).val() == 14);
static_assert((Float(9) / Float(2)).val() == 4.5);
static_assert((2 + Float(3)).val() == 5 && (Float(3) * 2.0).val() == 6);
static_assert((+Float(2)).val() == 2 && (-Float(2)).val() == -2);
static_assert(Float(0) == 5E-13 && !(Float(0) == 2E-12));
static_assert(Float(0) < 5E-13 && Float(0) >= 5E-13);
static_assert(Float(-eps).sgn() == 0 && Float(eps).sgn() == 0);
static_assert(std::is_same_v<decltype(std::declval<Float&>() += Float()), Float&>);
static_assert(std::is_same_v<decltype(std::declval<Float&>() -= Float()), Float&>);
static_assert(std::is_same_v<decltype(std::declval<Float&>() *= Float()), Float&>);
static_assert(std::is_same_v<decltype(std::declval<Float&>() /= Float()), Float&>);
static_assert(std::is_same_v<decltype(std::declval<Float>().round<int>()), int>);
static_assert(std::is_same_v<decltype(std::declval<Float>().round<long long>()), long long>);
static_assert(std::is_same_v<decltype(std::declval<std::istringstream&>() >> std::declval<Float&>()), std::istringstream&>);
static_assert(std::is_same_v<decltype(std::declval<std::ostringstream&>() << Float()), std::ostringstream&>);
#define CHECK_MATH_TYPE(fn) static_assert(std::is_same_v<decltype(std::fn(Float())), Float>)
CHECK_MATH_TYPE(abs);
CHECK_MATH_TYPE(sqrt);
CHECK_MATH_TYPE(sin);
CHECK_MATH_TYPE(cos);
CHECK_MATH_TYPE(tan);
CHECK_MATH_TYPE(asin);
CHECK_MATH_TYPE(acos);
CHECK_MATH_TYPE(atan);
#undef CHECK_MATH_TYPE
static_assert(std::is_same_v<decltype(std::atan2(Float(), Float())), Float>);
static_assert(std::is_same_v<decltype(std::fma(Float(), Float(), Float())), Float>);
static_assert(std::is_same_v<decltype(std::sin(0.5)), double>);

// Compare underlying doubles, never the wrapper's EPS-based equality.
void checkNumber(double actual, double expected, const char* operation, double a, double b = 0) {
    const bool same = std::isnan(expected) ? std::isnan(actual)
        : actual == expected && (expected != 0 || std::signbit(actual) == std::signbit(expected));
    if (!same) {
        std::cerr << std::setprecision(17) << operation << ": a=" << a << ", b=" << b
                  << ", actual=" << actual << ", expected=" << expected << '\n';
    }
    CHECK(same);
}

void checkArithmetic(double a, double b) {
    const Float x(a), y(b);
    checkNumber(x.val(), a, "val", a);
    checkNumber((+x).val(), a, "unary +", a);
    checkNumber((-x).val(), -a, "unary -", a);
    checkNumber((x + y).val(), a + b, "+", a, b);
    checkNumber((x - y).val(), a - b, "-", a, b);
    checkNumber((x * y).val(), a * b, "*", a, b);
    const double quotient = double(static_cast<long double>(a) / static_cast<long double>(b));
    checkNumber((x / y).val(), quotient, "/", a, b);
    Float value(a);
    CHECK(&(value += y) == &value);
    checkNumber(value.val(), a + b, "+=", a, b);
    value = x;
    CHECK(&(value -= y) == &value);
    checkNumber(value.val(), a - b, "-=", a, b);
    value = x;
    CHECK(&(value *= y) == &value);
    checkNumber(value.val(), a * b, "*=", a, b);
    value = x;
    CHECK(&(value /= y) == &value);
    checkNumber(value.val(), quotient, "/=", a, b);
    checkNumber((x + 2).val(), a + 2, "Float + int", a);
    checkNumber((2 + x).val(), 2 + a, "int + Float", a);
    checkNumber((x - 2).val(), a - 2, "Float - int", a);
    checkNumber((2 - x).val(), 2 - a, "int - Float", a);
    checkNumber((x * 2.0).val(), a * 2, "Float * double", a);
    checkNumber((2.0 * x).val(), 2 * a, "double * Float", a);
    checkNumber((x / 2).val(), double(static_cast<long double>(a) / 2), "Float / int", a);
    checkNumber((2 / x).val(), double(2 / static_cast<long double>(a)), "int / Float", a);
}

void checkComparisons(double a, double b) {
    const Float x(a), y(b);
    const bool equal = a == b || std::abs(a - b) <= eps;
    if ((x == y) != equal || (x < y) != (a < b) || (x > y) != (a > b)
        || (x != y) != !equal || (x <= y) != ((a < b) || equal)
        || (x >= y) != ((a > b) || equal)) {
        std::cerr << std::setprecision(17) << "comparison: a=" << a << ", b=" << b << '\n';
        CHECK(false);
    }
    CHECK((x == b) == equal && (a == y) == equal);
    CHECK((x != b) == !equal && (a != y) == !equal);
    CHECK((x < b) == (a < b) && (a < y) == (a < b));
    CHECK((x > b) == (a > b) && (a > y) == (a > b));
    CHECK((x <= b) == ((a < b) || equal) && (a <= y) == ((a < b) || equal));
    CHECK((x >= b) == ((a > b) || equal) && (a >= y) == ((a > b) || equal));
    const int sign = a < -eps ? -1 : (a > eps ? 1 : 0);
    CHECK(x.sgn() == sign);
}

void checkMath(double a, double b, double c) {
    const Float x(a), y(b), z(c);
#define CHECK_MATH_VALUE(fn) checkNumber(std::fn(x).val(), std::fn(a), #fn, a)
    CHECK_MATH_VALUE(abs);
    CHECK_MATH_VALUE(sqrt);
    CHECK_MATH_VALUE(sin);
    CHECK_MATH_VALUE(cos);
    CHECK_MATH_VALUE(tan);
    CHECK_MATH_VALUE(asin);
    CHECK_MATH_VALUE(acos);
    CHECK_MATH_VALUE(atan);
#undef CHECK_MATH_VALUE
    checkNumber(std::atan2(x, y).val(), std::atan2(a, b), "atan2", a, b);
    checkNumber(std::fma(x, y, z).val(), std::fma(a, b, c), "fma", a, b);
}

void checkRound(double input, long long expected) {
    const long long actual = Float(input).round<long long>();
    if (actual != expected) {
        std::cerr << std::setprecision(17) << "round: input=" << input << ", actual=" << actual
                  << ", expected=" << expected << '\n';
    }
    CHECK(actual == expected);
    if (expected >= std::numeric_limits<int>::min() && expected <= std::numeric_limits<int>::max())
        CHECK(Float(input).round<int>() == expected);
}

void testRound() {
    checkRound(0.4999999999999, 1);
    checkRound(-1.5, -1);
    checkRound(-0.4999999999999, 0);
    checkRound(-1.5000000000001, -1);
    checkRound(-0.6, -1);
    checkRound(-1.6, -2);
    // Each half-integer uses an independently known lower and upper integer.
    for (int lower = -100; lower <= 100; ++lower) {
        const double half = lower + 0.5;
        checkRound(lower, lower);
        checkRound(half, lower + 1);
        checkRound(std::nextafter(half, -std::numeric_limits<double>::infinity()), lower + 1);
        checkRound(std::nextafter(half, std::numeric_limits<double>::infinity()), lower + 1);
        checkRound(half - eps / 10, lower + 1);
        checkRound(half - eps / 2, lower + 1);
        checkRound(half - 2 * eps, lower);
        checkRound(half + 2 * eps, lower + 1);
    }
    const double threshold = 0.5 - eps;
    checkRound(threshold, 1);
    checkRound(std::nextafter(threshold, 0.0), 0);
    checkRound(std::nextafter(threshold, 1.0), 1);
    const double denorm = std::numeric_limits<double>::denorm_min();
    checkRound(0, 0);
    checkRound(-0.0, 0);
    checkRound(denorm, 0);
    checkRound(-denorm, 0);
    checkRound(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());
    checkRound(double(std::numeric_limits<int>::min()) - 0.5, std::numeric_limits<int>::min());
    checkRound(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    checkRound(double(std::numeric_limits<long long>::min()), std::numeric_limits<long long>::min());
    const double largest = std::nextafter(double(std::numeric_limits<long long>::max()), 0.0);
    checkRound(largest, static_cast<long long>(largest));
    // Large integral doubles must stay unchanged, including an odd integer at 2^52.
    for (double value : {4503599627370495.5, 4503599627370497.0, -4503599627370497.0})
        checkRound(value, static_cast<long long>(std::ceil(value)));
    CHECK(Float(0.4999999999999).round<unsigned>() == 1U);
    CHECK(Float(42.0).round<unsigned>() == 42U);
    // Exact dyadic inputs allow an integer oracle, with no floating rounding in it.
    for (int numerator = -8192; numerator <= 8192; ++numerator) {
        const int shifted = numerator + 8;
        const int expected = shifted >= 0 ? shifted / 16 : -((-shifted + 15) / 16);
        checkRound(numerator / 16.0, expected);
    }
    std::cout << "round: EPS boundaries, negative ties toward +infinity, integer limits, 16385 dyadic cases PASS\n";
}

void testStreams() {
    std::ostringstream initial;
    initial << Float(1.25);
    CHECK(initial.str() == "1.2500000000");
    std::istringstream input(" -1.25 +4.5e2 1.25suffix nan inf -inf ");
    Float a, b, partial, nan, inf, negativeInf;
    CHECK(&(input >> a) == &input);
    input >> b >> partial >> nan >> inf >> negativeInf;
    CHECK(a.val() == -1.25 && b.val() == 450 && partial.val() == 1.25);
    CHECK(std::isnan(nan.val()) && std::isinf(inf.val()) && inf.val() > 0 && negativeInf.val() < 0);
    Float unchanged(17);
    input >> unchanged;
    CHECK(input.fail() && unchanged.val() == 17);
    for (const std::string token : {"malformed", "1e9999", "1e-9999"}) {
        std::istringstream bad(token);
        bool invalid = false, outOfRange = false;
        try { bad >> unchanged; }
        catch (const std::invalid_argument&) { invalid = true; }
        catch (const std::out_of_range&) { outOfRange = true; }
        CHECK(unchanged.val() == 17);
        CHECK(token == "malformed" ? invalid && !outOfRange : outOfRange && !invalid);
        CHECK(!bad.fail());
    }
    std::istringstream failed("5.25");
    failed.setstate(std::ios::failbit);
    failed >> unchanged;
    CHECK(unchanged.val() == 17);
    std::istringstream valid("2.5 -0 6.25");
    Float zero;
    valid >> unchanged >> zero >> a;
    CHECK(unchanged.val() == 2.5 && zero.val() == 0 && std::signbit(zero.val()) && a.val() == 6.25);
    for (int precision : {0, 3, 6, 10, 17}) {
        Float::setprecision(precision);
        for (double value : {-0.0, -1.25, 0.0, 2.5, 123456.789}) {
            std::ostringstream actual, expected;
            CHECK(&(actual << Float(value)) == &actual);
            expected << std::fixed << std::setprecision(precision) << value;
            CHECK(actual.str() == expected.str());
            CHECK(formatFromOtherTranslationUnit(value) == expected.str());
            CHECK((actual.flags() & std::ios::floatfield) == std::ios::fixed);
            CHECK(actual.precision() == precision);
        }
    }
    Float::setprecision(17);
    for (int numerator = -1024; numerator <= 1024; ++numerator) {
        const double value = numerator / 16.0;
        std::stringstream stream;
        stream << Float(value);
        Float parsed;
        stream >> parsed;
        checkNumber(parsed.val(), value, "stream roundtrip", value);
    }
    Float::setprecision(10);
    std::cout << "streams: stod parsing/errors/EOF, signed zero, precision and two-TU sharing PASS\n";
}

int main() {
    testRound();
    testStreams();
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double denorm = std::numeric_limits<double>::denorm_min();
    const std::array<double, 19> boundaries = {{-inf, -std::numeric_limits<double>::max(),
        -1.0, -2 * eps, std::nextafter(-eps, -inf), -eps, -eps / 2, -denorm, -0.0,
        0.0, denorm, eps / 2, eps, std::nextafter(eps, inf), 2 * eps, 1.0,
        std::numeric_limits<double>::max(), inf, nan}};
    for (double a : boundaries) {
        for (double b : boundaries) {
            checkArithmetic(a, b);
            checkComparisons(a, b);
            checkMath(a, b, 1.0);
        }
    }
    std::array<Float, 6> ordered = {{Float(eps / 2), Float(-eps / 2), Float(0), Float(2), Float(-2), Float(eps)}};
    std::sort(ordered.begin(), ordered.end());
    for (std::size_t i = 1; i < ordered.size(); ++i)
        CHECK(ordered[i - 1].val() < ordered[i].val());
    // FMA must preserve the cancellation result that separate multiply/add loses.
    const double aboveOne = std::nextafter(1.0, 2.0), belowOne = std::nextafter(1.0, 0.0);
    const volatile double roundedProduct = aboveOne * belowOne;
    CHECK(std::fma(aboveOne, belowOne, -1.0) != roundedProduct - 1.0);
    checkMath(aboveOne, belowOne, -1.0);
    for (int i = 0; i < 100000; ++i) {
        const double a = randomInt(-100000, 100000) / 37.0;
        const double b = randomInt(-100000, 100000) / 31.0;
        const double c = randomInt(-100000, 100000) / 29.0;
        checkArithmetic(a, b);
        checkComparisons(a, b);
        checkMath(a, b, c);
        const int numerator = randomInt(-1000000, 1000000);
        const int shifted = numerator + 512;
        const int expected = shifted >= 0 ? shifted / 1024 : -((-shifted + 1023) / 1024);
        checkRound(numerator / 1024.0, expected);
    }
    for (int i = 0; i < 10000; ++i) {
        const double a = std::ldexp(double(randomInt(-1000000, 1000000)), randomInt(-900, 900));
        const double b = std::ldexp(double(randomInt(-1000000, 1000000)), randomInt(-900, 900));
        checkArithmetic(a, b);
        checkComparisons(a, b);
    }
    std::cout << "Float=FloatPointNumber<double>: constexpr/types, IEEE boundaries, comparisons, all std math, "
                 "100K random oracles and 10K exponent-range cases PASS; TEST_SEED=" << testSeed << '\n';
}
