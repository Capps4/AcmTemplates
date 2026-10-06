#include "../../../../src/Clarketech/ListHelper/code.hpp"
#include "../../../Support/CaseSupport.hpp"
#include <sstream>

int main() {
    runCase("ListHelper/01-empty", [] {
        std::vector<int> a;
        CHECK((a | sorted() | unique()).empty());
        CHECK(!(a | first([](int) {
                    return true;
                })));
    });
    runCase("ListHelper/02-sort-borrows", [] {
        std::vector<int> a{4, 1, 4, 2};
        auto b = a;
        CHECK((a | sorted()) == std::vector<int>({1, 2, 4, 4}));
        CHECK(a == b);
    });
    runCase("ListHelper/03-wide-signed", [] {
        std::vector<long long> a{LLONG_MAX, 0, LLONG_MIN, -1};
        auto e = a;
        std::sort(e.begin(), e.end());
        CHECK((a | sorted()) == e);
    });
    runCase("ListHelper/04-descending", [] {
        CHECK((std::vector<int>{2, 1, 3} | sorted(std::greater<int>{})) ==
              std::vector<int>({3, 2, 1}));
    });
    runCase("ListHelper/05-unique-adjacent", [] {
        CHECK((std::vector<int>{1, 1, 2, 1, 1} | unique()) == std::vector<int>({1, 2, 1}));
    });
    runCase("ListHelper/06-filter-map", [] {
        CHECK((std::vector<int>{-2, -1, 0, 1, 2} | filter([](int x) {
                   return x % 2 == 0;
               }) |
               map([](int x) {
                   return x * x;
               })) == std::vector<int>({4, 0, 4}));
    });
    runCase("ListHelper/07-slice-clamp", [] {
        CHECK((std::vector<int>{1, 2, 3} | slice(1, 99)) == std::vector<int>({2, 3}));
        CHECK((std::vector<int>{1} | slice(4, 0)).empty());
    });
    runCase("ListHelper/08-reusable-state", [] {
        auto op = map([k = 0](int) mutable {
            return ++k;
        });
        std::vector<int> a{0, 0};
        CHECK((a | op) == std::vector<int>({1, 2}));
        CHECK((a | op) == std::vector<int>({3, 4}));
    });
    runCase("ListHelper/09-temporary-member", [] {
        auto s = std::vector<std::string>{std::string(4096, 'x')} | call(front);
        CHECK(s == std::string(4096, 'x'));
    });
    runCase("ListHelper/10-proxy-stream", [] {
        std::vector<bool> a{true, false, true};
        CHECK((a | map([](bool x) {
                   return !x;
               })) == std::vector<bool>({false, true, false}));
        std::ostringstream out;
        a | seq::cout(out, "|", ".");
        CHECK(out.str() == "1|0|1.");
    });
    return finishCases(10);
}
