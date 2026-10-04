#include "Final.hpp"
#include "../FloatPointNumber/Final.hpp"
#include "../Geo2/PointVec.hpp"
#include "../ListHelper/Final.hpp"
#include "../TestSupport.hpp"
#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
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
int main() {
    static_assert(!std::is_copy_constructible_v<QInput> && !std::is_move_constructible_v<QInput>);
    static_assert(!std::is_copy_constructible_v<QOutput> && !std::is_move_constructible_v<QOutput>);
    std::vector<long long> expected{0, -1, 1, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max()};
    for (int i = 0; i < 100000; ++i) expected.push_back(static_cast<long long>(testRng()));
    std::ostringstream text;
    for (auto value : expected) text << value << " \t\r\n";
    FILE* file = inputFile(text.str());
    auto reader = std::make_unique<QInput>(file);
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

    file = inputFile("0 1 2"); reader = std::make_unique<QInput>(file);
    bool a = true, b = false, c = false;
    *reader >> a >> b >> c;
    CHECK(!a && b && !c && reader->fail());
    reader.reset(); std::fclose(file);
    constexpr int bufferSize = 1 << 20;
    file = inputFile(std::string(bufferSize - 1, ' ') + "-2147483648\n" + std::string(bufferSize + 7, 'x') + " end");
    reader = std::make_unique<QInput>(file);
    int minimum = 0; *reader >> minimum;
    std::string big, end; *reader >> big >> end;
    CHECK(minimum == std::numeric_limits<int>::min() && big == std::string(bufferSize + 7, 'x') && end == "end");
    reader.reset(); std::fclose(file);
    file = inputFile(std::string("\nfirst\0second", 13)); reader = std::make_unique<QInput>(file);
    std::string line;
    reader->getLine(line); CHECK(line.empty() && *reader);
    reader->getLine(line, '\0'); CHECK(line == "first" && *reader);
    reader->getLine(line); CHECK(line == "second" && *reader);
    reader->getLine(line); CHECK(reader->fail());
    reader.reset(); std::fclose(file);

    file = inputFile("-1.25 2.5e2 3 4 8 9 10 malformed"); reader = std::make_unique<QInput>(file);
    double raw = 0; FloatPointNumber<double> wrapped; Point<i64> point;
    *reader >> raw >> wrapped >> point;
    CHECK(raw == -1.25 && wrapped == 250 && point == Point<i64>(3, 4));
    auto data = std::vector<int>(3) | readFrom(*reader);
    CHECK(data == std::vector<int>({8, 9, 10}));
    wrapped = 17;
    bool invalid = false;
    try { *reader >> wrapped; }
    catch (const std::invalid_argument&) { invalid = true; }
    CHECK(invalid && wrapped == 17);
    reader.reset(); std::fclose(file);

    file = std::tmpfile(); CHECK(file);
    auto writer = std::make_unique<QOutput>(file);
    std::ostringstream reference;
    for (auto value : expected) { *writer << value << '\n'; reference << value << '\n'; }
    *writer << std::string_view("view") << ' ' << true << ' ' << false << std::endl;
    reference << "view 1 0\n";
    writer->writeReal(FloatPointNumber<double>(1.25).val(), 3) << ' ' << Point<i64>(3, 4) << '\n';
    reference << "1.250 (3, 4)\n";
    data | writeTo(*writer, ",", "!"); reference << "8,9,10!";
    *writer << std::ends << std::flush; reference << '\0';
    CHECK(*writer);
    writer.reset(); // Destructor flushes before the borrowed FILE is closed.
    CHECK(contents(file) == reference.str());
    std::fclose(file);

    file = std::tmpfile(); CHECK(file); writer = std::make_unique<QOutput>(file);
    unsigned __int128 huge = ~static_cast<unsigned __int128>(0);
    __int128 low = -(__int128(1) << 126) * 2;
    *writer << huge << ' ' << low << '\n'; writer.reset(); std::rewind(file);
    reader = std::make_unique<QInput>(file);
    unsigned __int128 actualHuge = 0; __int128 actualLow = 0;
    *reader >> actualHuge >> actualLow;
    CHECK(*reader && actualHuge == huge && actualLow == low);
    reader.reset(); std::fclose(file);
    file = std::fopen("/dev/null", "r"); CHECK(file); writer = std::make_unique<QOutput>(file);
    *writer << "error"; writer->flush(); CHECK(writer->fail()); writer.reset(); std::fclose(file);
    file = std::fopen("/dev/null", "w"); CHECK(file); reader = std::make_unique<QInput>(file);
    int error = 17; *reader >> error; CHECK(reader->fail() && error == 17); reader.reset(); std::fclose(file);
    std::cout << "FastInputOutput 100K integer/128-bit roundtrip, 1MB boundaries, EOF/errors/lines, Float/Point/ListHelper integration PASS\n";
}
