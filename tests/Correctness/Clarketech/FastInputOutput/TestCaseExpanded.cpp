#include "../../../../src/Clarketech/FastInputOutput/code.hpp"
#undef cin
#undef cout
#include "../../../Support/CaseSupport.hpp"
#undef cin
#undef cout
#include <sstream>
void verifyAdded(const std::vector<long long> &a, std::string_view sep) {
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
            CHECK(got == x);
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

int main() {
    runCase("FastInputOutput/01-roundtrip-00", [] {
        verifyAdded({}, " ");
    });
    runCase("FastInputOutput/02-roundtrip-01", [] {
        verifyAdded({0}, " ");
    });
    runCase("FastInputOutput/03-roundtrip-02", [] {
        verifyAdded({-1, 1}, "\n");
    });
    runCase("FastInputOutput/04-roundtrip-03", [] {
        verifyAdded({LLONG_MIN, 9223372036854775807}, "\t");
    });
    runCase("FastInputOutput/05-roundtrip-04", [] {
        verifyAdded({123456789012345, -123456789012345}, "\r\n");
    });
    runCase("FastInputOutput/06-roundtrip-05", [] {
        verifyAdded({1, 22, 333, 4444, 55555}, "\u000b");
    });
    runCase("FastInputOutput/07-roundtrip-06", [] {
        verifyAdded(std::vector<long long>(5000, 0), " ");
    });
    runCase("FastInputOutput/08-roundtrip-07", [] {
        verifyAdded({2147483647, -2147483648}, "\f");
    });
    runCase("FastInputOutput/09-roundtrip-08", [] {
        verifyAdded({-7, 0, 7}, std::string((1 << 20) - 1, ' '));
    });
    runCase("FastInputOutput/10-roundtrip-09", [] {
        verifyAdded({123456789}, "");
    });
    return finishCases(10);
}
