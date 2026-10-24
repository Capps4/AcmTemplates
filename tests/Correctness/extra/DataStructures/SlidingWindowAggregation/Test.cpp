#include "../../../../../src/extra/DataStructures/SlidingWindowAggregation/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("SlidingWindowAggregation/corners", [] {
        struct Text {
            std::string value;
            explicit Text(std::string value) : value(std::move(value)) {}
        };
        auto join = [reverse = false](const Text& a, const Text& b) {
            return Text(reverse ? b.value + a.value : a.value + b.value);
        };
        SWAG<Text, decltype(join)> q(join);
        CHECK(q.empty());
        CHECK(q.size() == 0);
        q.push(Text("a"));
        CHECK(q.query().value == "a");
        q.push(Text("bc"));
        q.push(Text("d"));
        CHECK(q.query().value == "abcd");
        q.pop();
        CHECK(q.query().value == "bcd");
        q.push(Text("ef"));
        CHECK(q.query().value == "bcdef");
        q.pop();
        CHECK(q.query().value == "def");
        q.pop();
        CHECK(q.query().value == "ef");
        q.pop();
        CHECK(q.empty());
        q.push(Text(""));
        CHECK(q.query().value.empty());
        CHECK(q.size() == 1);
        q.pop();
        CHECK(q.empty());
    });
    runCase("SlidingWindowAggregation/order-oracle", [] {
        struct Affine {
            int a, b;
        };
        auto compose = [](Affine x, Affine y) {
            return Affine{y.a * x.a % 101, (y.a * x.b + y.b) % 101};
        };
        SWAG<Affine, decltype(compose)> q(compose);
        std::deque<Affine> values;
        for (int step = 0; step < 2000; ++step) {
            test_context::step = step;
            if (values.empty() or (values.size() < 64 and randomInt(0, 1))) {
                Affine value{randomInt(0, 100), randomInt(0, 100)};
                values.push_back(value);
                q.push(value);
            } else {
                values.pop_front();
                q.pop();
            }
            CHECK(q.size() == values.size());
            CHECK(q.empty() == values.empty());
            if (values.empty()) {
                continue;
            }
            auto actual = q.query();
            for (int input : {0, 1, 37, 100}) {
                int expected = input;
                for (auto f : values) {
                    expected = (f.a * expected + f.b) % 101;
                }
                CHECK((actual.a * input + actual.b) % 101 == expected);
            }
        }
    });
    runCase("SlidingWindowAggregation/large", [] {
        constexpr int n = 100000;
        SWAG<long long> q;
        for (int i = 0; i < n; ++i) {
            q.push(i);
        }
        long long expected = 1LL * n * (n - 1) / 2;
        for (int i = 0; i < n; ++i) {
            CHECK(q.query() == expected);
            q.pop();
            q.push(n + i);
            expected += n;
        }
        for (int i = n; i < 2 * n; ++i) {
            CHECK(q.query() == expected);
            q.pop();
            expected -= i;
        }
        CHECK(q.empty());
        CHECK(expected == 0);
    });
    return 0;
}
