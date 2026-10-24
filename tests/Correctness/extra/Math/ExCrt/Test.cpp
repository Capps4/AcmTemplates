#include "../../../../../src/extra/Math/ExCrt/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("ExCrt/oracle", [] {
        for (int it = 0; it < 100; ++it) {
            ExCrt<long long> a;
            std::vector<std::pair<int,int>> eq;
            int mod = 1;
            for (int j = 0; j < 3; ++j) {
                int m = randomInt(1, 10), x = randomInt(-20, 20);
                eq.emplace_back(x, m);
                a.insert(x, m);
                mod = std::lcm(mod, m);
            }
            long long want = -1;
            for (int x = 0; x < mod; ++x) {
                bool ok = true;
                for (auto [v,m] : eq) {
                    ok = ok and (x - v) % m == 0;
                }
                if (ok) { want = x; break; }
            }
            CHECK(a.get() == want and a.get() == 0);
        }
        ExCrt<long long> a;
        a.insert(0, std::numeric_limits<long long>::max());
        a.insert(0, 2);
        bool caught = false;
        try { (void)a.get(); } catch (const std::overflow_error&) { caught = true; }
        CHECK(caught and a.get() == 0);

        a.insert(std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());
        CHECK(a.get() == std::numeric_limits<long long>::max() - 1);
        const long long first = 1000000007, second = 1000000009;
        const long long expected = first * second - 17;
        a.insert(expected % first, first);
        a.insert(expected % second, second);
        CHECK(a.get() == expected);

        a.insert(-1, 6);
        a.insert(9, 10);
        CHECK(a.get() == 29);
        a.insert(0, 6);
        a.insert(1, 10);
        CHECK(a.get() == -1);
        a.insert(7, 9);
        CHECK(a.get() == 7 and a.get() == 0);
    });
    return 0;
}
