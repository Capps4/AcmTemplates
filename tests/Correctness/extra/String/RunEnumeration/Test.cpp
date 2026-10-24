#include "../../../../../src/extra/String/RunEnumeration/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using Run = std::tuple<int, int, int>;

std::vector<Run> oracle(std::string_view s) {
    int n = s.size();
    std::vector<Run> answer;
    for (int l = 0; l < n; ++l) {
        for (int r = l + 2; r <= n; ++r) {
            for (int p = 1; 2 * p <= r - l; ++p) {
                bool valid = true;
                for (int i = l + p; i < r; ++i) {
                    if (s[i] != s[i - p]) {
                        valid = false;
                        break;
                    }
                }
                if (not valid)
                    continue;
                if ((l == 0 or s[l - 1] != s[l - 1 + p]) and
                    (r == n or s[r] != s[r - p]))
                    answer.emplace_back(p, l, r);
                break;
            }
        }
    }
    std::sort(answer.begin(), answer.end());
    return answer;
}

void exhaustive(int limit) {
    for (int n = 0; n <= limit; ++n) {
        for (int mask = 0; mask < (1 << n); ++mask) {
            std::string s(n, 'a');
            for (int i = 0; i < n; ++i)
                s[i] += (mask >> i) & 1;
            test_context::describe(s);
            CHECK(runEnumeration(s) == oracle(s));
        }
    }
}

void random(int count, int limit) {
    for (int trial = 0; trial < count; ++trial) {
        test_context::step = trial;
        int n = randomInt(0, limit);
        std::string s(n, '\0');
        for (auto &c : s)
            c = char(randomInt(0, trial % 3 ? 3 : 255));
        test_context::describe(s);
        auto runs = runEnumeration(s);
        CHECK(runs == oracle(s));
        std::reverse(s.begin(), s.end());
        auto mirrored = runs;
        for (auto &[p, l, r] : mirrored) {
            (void)p;
            int end = r;
            r = n - l;
            l = n - end;
        }
        std::sort(mirrored.begin(), mirrored.end());
        CHECK(runEnumeration(s) == mirrored);
        std::reverse(s.begin(), s.end());
        for (auto &c : s)
            c = char((unsigned char)c ^ 0xa5);
        CHECK(runEnumeration(s) == runs);
    }
}

void large(int n) {
    CHECK(runEnumeration(std::string(n, 'a')) == std::vector<Run>({{1, 0, n}}));
    std::string s(n, 'a');
    for (int i = 0; i < n; ++i)
        s[i] = char(i % 3);
    CHECK(runEnumeration(s) == std::vector<Run>({{3, 0, n}}));
    std::string a = "a", b = "ab";
    for (int i = 0; i < 13; ++i) {
        std::string c = b + a;
        a = b;
        b = c;
    }
    auto runs = runEnumeration(b);
    CHECK(runs.size() < b.size());
    for (auto [p, l, r] : runs) {
        CHECK(0 <= l and r <= int(b.size()) and r - l >= 2 * p);
        for (int i = l + p; i < r; ++i)
            CHECK(b[i] == b[i - p]);
        CHECK(l == 0 or b[l - 1] != b[l - 1 + p]);
        CHECK(r == int(b.size()) or b[r] != b[r - p]);
        for (int q = 1; q < p; ++q) {
            bool valid = true;
            for (int i = l + q; i < r; ++i) {
                if (b[i] != b[i - q]) {
                    valid = false;
                    break;
                }
            }
            CHECK(not valid);
        }
    }
}

int main() {
    runCase("RunEnumeration/exhaustive-substrings", [] {
        exhaustive(9);
    });
    runCase("RunEnumeration/random-bytes-and-symmetry", [] {
        random(120, 32);
    });
    runCase("RunEnumeration/large-periodic-and-nested", [] {
        large(10000);
    });
    runStressCase("RunEnumeration/full-enumeration-and-periodic-stress", [] {
        exhaustive(12);
        random(600, 45);
        large(100000);
    });
    return 0;
}
