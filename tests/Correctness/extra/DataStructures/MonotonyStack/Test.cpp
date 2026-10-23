#include "../../../../../src/extra/DataStructures/MonotonyStack/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("MonotonyStack/oracle", [] {
for (int it = 0; it < 20; ++it) {
            MonotonyStack<int> st(-1,-1);
            std::vector<int> a(80);
            for (int i = 0; i < int(a.size()); ++i) {
                a[i] = randomInt(0,20);
                int j = i - 1;
                while (j >= 0 and a[j] >= a[i]) { --j; }
                CHECK(st.push(a[i], i) == j);
            }
        }
    });
    return 0;
}
