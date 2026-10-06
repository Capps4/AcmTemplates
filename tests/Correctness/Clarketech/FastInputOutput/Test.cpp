#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Math/MathPackage/FloatPointNumber/code.hpp"
#include "../../../../src/Geometry/Geo2/PointVec.hpp"
#include "../../../../src/Clarketech/ListHelper/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include "../../../../src/Clarketech/FastInputOutput/code.hpp"

using i64 = long long;
FILE* inputFile(const std::string& text) {
    FILE* file = std::tmpfile();
    CHECK(file);
    CHECK(std::fwrite(text.data(), 1, text.size(), file) == text.size());
    std::rewind(file);
    return file;
}
std::string contents(FILE* file) {
    std::rewind(file);
    std::string result; char block[4096];
    while (std::size_t count = std::fread(block, 1, sizeof(block), file)) result.append(block, count);
    return result;
}
int coreCases() {
    static_assert(std::is_same_v<decltype(cin), Qinput&>);
    static_assert(std::is_same_v<decltype(cout), Qoutput&>);
    CHECK(&cin == &Qinput::shared() and &cout == &Qoutput::shared());
    static_assert(!std::is_copy_constructible_v<Qinput> && !std::is_move_constructible_v<Qinput>);
    static_assert(!std::is_copy_constructible_v<Qoutput> && !std::is_move_constructible_v<Qoutput>);
    std::vector<long long> expected{0, -1, 1, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max()};
    for (int i = 0; i < 48; ++i) expected.push_back(static_cast<long long>(testRng()));
    std::ostringstream text;
    for (auto value : expected) text << value << " \t\r\n\f\v";
    FILE* file = inputFile(text.str());
    auto reader = std::make_unique<Qinput>(file);
    for (auto value : expected) {
        long long result = 77;
        *reader >> result;
        CHECK(*reader && result == value);
    }
    long long unchanged = 99;
    *reader >> unchanged;
    CHECK(reader->fail() && reader->eof() && unchanged == 99);
    std::rewind(file); reader->clear();
    // Buffered unread state has no data after EOF; clear allows a rewound FILE.
    *reader >> unchanged;
    CHECK(*reader && unchanged == 0);
    reader.reset(); std::fclose(file);

    constexpr int bufferSize = 1 << 20;
    file = inputFile(std::string(bufferSize - 1, ' ') + "-2147483648\n" + std::string(bufferSize + 7, 'x') + " end");
    reader = std::make_unique<Qinput>(file);
    int minimum = 0; *reader >> minimum;
    std::string big, end; *reader >> big >> end;
    CHECK(minimum == std::numeric_limits<int>::min() && big == std::string(bufferSize + 7, 'x') && end == "end");
    reader.reset(); std::fclose(file);
    file = inputFile(std::string("\nfirst\0second", 13)); reader = std::make_unique<Qinput>(file);
    std::string line;
    reader->getLine(line); CHECK(line.empty() && *reader);
    reader->getLine(line, '\0'); CHECK(line == "first" && *reader);
    reader->getLine(line); CHECK(line == "second" && *reader);
    reader->getLine(line); CHECK(reader->fail());
    reader.reset(); std::fclose(file);

    file = std::tmpfile(); CHECK(file);
    auto writer = std::make_unique<Qoutput>(file);
    std::ostringstream reference;
    for (auto value : expected) { *writer << value << '\n'; reference << value << '\n'; }
    *writer << "view" << ' ' << true << ' ' << false << std::endl;
    reference << "view 1 0\n";
    *writer << 8 << ',' << 9 << ',' << 10 << '!'; reference << "8,9,10!";
    *writer << std::ends << std::flush; reference << '\0';
    CHECK(*writer);
    writer.reset(); // Destructor flushes before the borrowed FILE is closed.
    CHECK(contents(file) == reference.str());
    std::fclose(file);

    file = std::tmpfile(); CHECK(file); writer = std::make_unique<Qoutput>(file);
    unsigned __int128 huge = ~static_cast<unsigned __int128>(0);
    __int128 low = -(__int128(1) << 126) * 2;
    *writer << huge << ' ' << low << '\n'; writer.reset(); std::rewind(file);
    reader = std::make_unique<Qinput>(file);
    unsigned __int128 actualHuge = 0; __int128 actualLow = 0;
    *reader >> actualHuge >> actualLow;
    CHECK(*reader && actualHuge == huge && actualLow == low);
    reader.reset(); std::fclose(file);
    file = std::fopen("/dev/null", "r"); CHECK(file); writer = std::make_unique<Qoutput>(file);
    *writer << "error"; writer->flush(); CHECK(writer->fail()); writer.reset(); std::fclose(file);
    file = std::fopen("/dev/null", "w"); CHECK(file); reader = std::make_unique<Qinput>(file);
    int error = 17; *reader >> error; CHECK(reader->fail() && error == 17); reader.reset(); std::fclose(file);
    std::cout << "FastInputOutput integer/128-bit roundtrip, 1MB boundaries, EOF/errors/lines, exact macros PASS\n";
    return 0;
}

#include "../../../../src/Clarketech/FastInputOutput/code.hpp"
#undef cin
#undef cout
#include "../../../Support/CaseSupport.hpp"
#undef cin
#undef cout
#include <sstream>

namespace boundary_cases {
void verifyAdded(const std::vector<long long> &a, std::string_view sep) {
    test_context::describe(a);
    FILE *f = std::tmpfile();
    CHECK(f != nullptr);
    std::string text;
    for (auto x : a) {
        text += std::to_string(x);
        text += sep;
    }
    CHECK(std::fwrite(text.data(), 1, text.size(), f) == text.size());
    std::rewind(f);
    {
        auto in = std::make_unique<Qinput>(f);
        for (auto x : a) {
            long long got = 13;
            *in >> got;
            CHECK(bool(*in));
            CHECK_EQ(got, x);
        }
        long long got = 19;
        *in >> got;
        CHECK(in->fail());
        CHECK(got == 19);
    }
    std::fclose(f);
    f = std::tmpfile();
    CHECK(f != nullptr);
    {
        auto out = std::make_unique<Qoutput>(f);
        for (auto x : a)
            *out << x << ' ';
        out->flush();
    }
    std::rewind(f);
    std::string got;
    char buf[1024];
    for (std::size_t n; (n = std::fread(buf, 1, sizeof(buf), f)) != 0;)
        got.append(buf, n);
    std::fclose(f);
    std::string expected;
    for (auto x : a)
        expected += std::to_string(x) + " ";
    CHECK(got == expected);
}

int run() {
    runCase("FastInputOutput/empty-eof", [] {
        verifyAdded({}, " ");
    });
    runCase("FastInputOutput/buffer-crossing", [] {
        verifyAdded({-7, 0, 7}, std::string((1 << 20) - 1, ' '));
    });
    runCase("FastInputOutput/eof-after-token", [] {
        verifyAdded({123456789}, "");
    });
    return 0;
}
}

int main() {
    runCase("FastInputOutput/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
