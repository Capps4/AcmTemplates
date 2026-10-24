#include "../../../../../src/extra/String/LyndonFactorization/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int compare(std::string_view a, std::string_view b) {
    std::size_t i = 0;
    while (i < a.size() and i < b.size()) {
        auto x = (unsigned char)a[i], y = (unsigned char)b[i];
        if (x != y)
            return x < y ? -1 : 1;
        ++i;
    }
    return a.size() < b.size() ? -1 : a.size() != b.size();
}

std::vector<int> oracle(std::string_view s) {
    int n = s.size(), count = 0;
    std::vector<int> path{0}, answer;
    auto dfs = [&](auto &self, int l) -> void {
        if (l == n) {
            ++count;
            answer = path;
            return;
        }
        for (int r = l + 1; r <= n; ++r) {
            auto word = s.substr(l, r - l);
            bool valid = true;
            for (int i = l + 1; i < r; ++i) {
                if (compare(word, s.substr(i, r - i)) >= 0)
                    valid = false;
            }
            if (path.size() > 1) {
                int p = path[path.size() - 2];
                if (compare(s.substr(p, l - p), word) < 0)
                    valid = false;
            }
            if (not valid)
                continue;
            path.push_back(r);
            self(self, r);
            path.pop_back();
        }
    };
    dfs(dfs, 0);
    CHECK_EQ(count, 1);
    return answer;
}

int main() {
    runCase("LyndonFactorization/exhaustive-partitions", [] {
        for (int n = 0; n <= 11; ++n) {
            for (int mask = 0; mask < (1 << n); ++mask) {
                std::string s(n, 'a');
                for (int i = 0; i < n; ++i)
                    s[i] += (mask >> i) & 1;
                test_context::describe(s);
                CHECK(lyndonFactorization(s) == oracle(s));
            }
        }
    });
    runCase("LyndonFactorization/random-bytes", [] {
        for (int trial = 0; trial < 500; ++trial) {
            test_context::step = trial;
            std::string s(randomInt(0, 16), '\0');
            for (auto &c : s)
                c = char(randomInt(0, trial % 2 ? 255 : 3));
            test_context::describe(s);
            CHECK(lyndonFactorization(s) == oracle(s));
        }
        CHECK(lyndonFactorization(std::string("\xff\0", 2)) == std::vector<int>({0, 1, 2}));
        CHECK(lyndonFactorization(std::string("\0\xff", 2)) == std::vector<int>({0, 2}));
    });
    runCase("LyndonFactorization/large-extreme-factors", [] {
        int n = 200000;
        auto pos = lyndonFactorization(std::string(n, 'x'));
        CHECK_EQ(int(pos.size()), n + 1);
        for (int i = 0; i <= n; ++i)
            CHECK_EQ(pos[i], i);
        std::string s(n, 'b');
        s[0] = 'a';
        CHECK(lyndonFactorization(s) == std::vector<int>({0, n}));
        for (int i = 0; i < n; ++i)
            s[i] = char('a' + i % 2);
        pos = lyndonFactorization(s);
        CHECK_EQ(int(pos.size()), n / 2 + 1);
        for (int i = 0; i <= n / 2; ++i)
            CHECK_EQ(pos[i], 2 * i);
    });
    return 0;
}
