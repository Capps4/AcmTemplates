#include "../../../../../src/Math/MathPackage/Combinatorics/code.hpp"
#include "../../../../Support/TestSupport.hpp"
using Small = ModuloInteger<int, 97>;
using Large = ModuloInteger<long long, 2305843009213693951LL>;
using Dynamic = ModuloInteger<int, 0>;
template <class T>
void checkPascal(int limit) {
    Comb<T> c;
    std::vector<T> row{1};
    T factorial = 1;
    for (int n = 0; n <= limit; ++n) {
        CHECK(c.jc(n) == factorial);
        CHECK(c.ijc(n) * factorial == T(1));
        for (int k = -1; k <= n + 1; ++k) {
            CHECK(c.C(n, k) == (k < 0 || k > n ? T(0) : row[k]));
            T permutation = 1;
            if (k < 0 || k > n)
                permutation = 0;
            else
                for (int i = 0; i < k; ++i)
                    permutation *= n - i;
            CHECK(c.A(n, k) == permutation);
        }
        row.push_back(0);
        for (int k = n + 1; k > 0; --k)
            row[k] += row[k - 1];
        factorial *= n + 1;
        // Repeated and smaller initialization must preserve existing entries.
        c.init(n);
        c.init(0);
    }
    CHECK(c.C(-1, 0) == T(0));
}
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("Combinatorics/01-zero", [] {
        checkPascal<Small>(0);
    });
    runCase("Combinatorics/02-one", [] {
        checkPascal<Small>(1);
    });
    runCase("Combinatorics/03-prime-boundary", [] {
        checkPascal<Small>(96);
    });
    runCase("Combinatorics/04-wide-modulus", [] {
        checkPascal<Large>(37);
    });
    runCase("Combinatorics/05-shared-grow", [] {
        auto &c = Comb<Z>::shared(150);
        CHECK(c.C(150, 0) == Z(1));
        CHECK(c.C(150, 150) == Z(1));
    });
    runCase("Combinatorics/06-symmetry", [] {
        Comb<Z> c(64);
        for (int k = 0; k <= 64; ++k)
            CHECK(c.C(64, k) == c.C(64, 64 - k));
    });
    runCase("Combinatorics/07-invalid-k", [] {
        Comb<Z> c(8);
        CHECK(c.C(8, -2) == Z(0));
        CHECK(c.C(8, 9) == Z(0));
        CHECK(c.A(-1, 0) == Z(0));
    });
    runCase("Combinatorics/08-dynamic-switch", [] {
        Dynamic::setMod(101);
        Comb<Dynamic> c(90);
        Dynamic::setMod(97);
        CHECK(c.C(10, 5) == Dynamic(252));
    });
    runCase("Combinatorics/09-permutation", [] {
        Comb<Z> c(12);
        CHECK(c.A(12, 4) == Z(11880));
    });
    runCase("Combinatorics/10-grow-shrink", [] {
        Comb<Z> c(1);
        c.init(80);
        auto v = c.C(80, 40);
        c.init(3);
        CHECK(c.C(80, 40) == v);
    });
    return finishCases(10);
}
