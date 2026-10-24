#include "../../../../../src/extra/Sequence/ImplicitTreap/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

struct Word {
    std::string text;
    explicit Word(std::string text) : text(std::move(text)) {}
    Word operator+(const Word& b) const { return Word(text + b.text); }
};

int main() {
    runCase("ImplicitTreap/corners", [] {
        ImplicitTreap<Word> init(std::vector<Word>{Word("a"), Word("bc"), Word("d")}, 123);
        CHECK(init.query(0, 3).text == "abcd");
        init.reverse(0, 3);
        CHECK(init.query(0, 3).text == "dbca");
        ImplicitTreap<Word> t(123);
        CHECK(t.empty() and t.size() == 0);
        t.erase(0, 0);
        t.reverse(0, 0);
        t.insert(0, Word("a"));
        t.insert(1, Word("b"));
        t.insert(1, Word("c"));
        CHECK(t.query(0, 3).text == "acb");
        t.reverse(0, 3);
        CHECK(t.query(0, 3).text == "bca");
        t.reverse(1, 3);
        CHECK(t.query(0, 3).text == "bac");
        t.erase(1, 2);
        CHECK(t.query(0, 2).text == "bc");
        auto copy = t;
        copy.insert(1, Word("x"));
        CHECK(copy.query(0, 3).text == "bxc");
        CHECK(t.query(0, 2).text == "bc");
        t.erase(0, 2);
        CHECK(t.empty());
        t.insert(0, Word("z"));
        CHECK(t.query(0, 1).text == "z");
    });
    runCase("ImplicitTreap/noncommutative-oracle", [] {
        for (int trial = 0; trial < 20; ++trial) {
            ImplicitTreap<Word> t(testRng());
            std::vector<std::string> a;
            for (int step = 0; step < 500; ++step) {
                test_context::step = step;
                int op = randomInt(0, 3), n = int(a.size());
                if (n == 0 or (op == 0 and n < 50)) {
                    int p = randomInt(0, n);
                    std::string v(1, char('a' + randomInt(0, 5)));
                    t.insert(p, Word(v));
                    a.insert(a.begin() + p, v);
                } else {
                    int l = randomInt(0, n), r = randomInt(l, n);
                    if (op == 1) {
                        t.erase(l, r);
                        a.erase(a.begin() + l, a.begin() + r);
                    } else {
                        t.reverse(l, r);
                        std::reverse(a.begin() + l, a.begin() + r);
                    }
                }
                CHECK(t.size() == int(a.size()));
                CHECK(t.empty() == a.empty());
                if (not a.empty()) {
                    int l = randomInt(0, int(a.size()) - 1);
                    int r = randomInt(l + 1, int(a.size()));
                    std::string expected;
                    for (int i = l; i < r; ++i) {
                        expected += a[i];
                    }
                    CHECK(t.query(l, r).text == expected);
                    std::string all;
                    for (const auto& v : a) {
                        all += v;
                    }
                    CHECK(t.query(0, t.size()).text == all);
                }
            }
        }
    });
    runCase("ImplicitTreap/large-recycle", [] {
        constexpr int n = 20000;
        std::vector<Info> a;
        for (int i = 0; i < n; ++i) {
            a.emplace_back(i);
        }
        ImplicitTreap<Info> t(a, 456);
        CHECK(t.query(0, n).val == 1LL * n * (n - 1) / 2);
        t.reverse(0, n);
        for (int i = 0; i < n; i += 137) {
            CHECK(t.query(i, i + 1).val == n - i - 1);
        }
        for (int pass = 0; pass < 3; ++pass) {
            t.erase(0, t.size());
            for (int i = 0; i < n; ++i) {
                t.insert(0, Info(i));
            }
            CHECK(t.query(0, n).val == 1LL * n * (n - 1) / 2);
        }
    });
    return 0;
}
