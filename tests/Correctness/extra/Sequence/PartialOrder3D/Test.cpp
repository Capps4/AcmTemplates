#include "../../../../../src/extra/Sequence/PartialOrder3D/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("PartialOrder3D/oracle", [] {
CHECK((partialOrder<3,long long>({},{}).empty()));
        for (int it = 0; it < 30; ++it) {
            int n = randomInt(1,40);
            std::vector<std::array<int,3>> a(n);
            std::vector<int> w(n);
            for (int i = 0; i < n; ++i) {
                for (auto& x : a[i]) { x = randomInt(-2,2); }
                w[i] = randomInt(-4,4);
            }
            auto got = partialOrder<3,long long>(a,w);
            for (int i = 0; i < n; ++i) {
                long long want = 0;
                for (int j = 0; j < n; ++j) {
                    if (a[j][0] <= a[i][0] and a[j][1] <= a[i][1] and a[j][2] <= a[i][2]) { want += w[j]; }
                }
                CHECK(got[i] == want);
            }
        }
    });
    return 0;
}
