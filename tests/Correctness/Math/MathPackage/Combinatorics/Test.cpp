#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/MathPackage/Combinatorics/code.hpp"
#include "../../../../Support/TestSupport.hpp"

using Small = ModuloInteger<int, 97>;
using Large = ModuloInteger<long long, 2305843009213693951LL>;
using Dynamic = ModuloInteger<int, 0>;
template<class T>
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
            if (k < 0 || k > n) permutation = 0;
            else for (int i = 0; i < k; ++i) permutation *= n - i;
            CHECK(c.A(n, k) == permutation);
        }
        row.push_back(0);
        for (int k = n + 1; k > 0; --k) row[k] += row[k - 1];
        factorial *= n + 1;
        // Repeated and smaller initialization must preserve existing entries.
        c.init(n); c.init(0);
    }
    CHECK(c.C(-1, 0) == T(0));
}
int coreCases() {
    checkPascal<Small>(96); checkPascal<Z>(150); checkPascal<Large>(70);
    Dynamic::setMod(101);
    Comb<Dynamic> dynamic(90);
    CHECK(dynamic.C(90, 45) == Dynamic(59)); // Python math.comb(90,45) % 101.
    for (int modulus : {97, 107, 101, 97}) {
        Dynamic::setMod(modulus);
        CHECK(dynamic.jc(10) == Dynamic(3628800));
        CHECK(dynamic.C(10, 5) == Dynamic(252));
        checkPascal<Dynamic>(modulus - 1);
    }
    static_assert(std::is_reference_v<decltype(comb)>);
    CHECK(&comb == &Comb<Z>::shared());
    CHECK(&Comb<Small>::shared(50) == &Comb<Small>::shared(80));
    CHECK(Comb<Small>::shared().C(90, 45) == Comb<Small>(96).C(90, 45));
    std::cout << "Combinatorics: Pascal oracle, factorials, 64-bit mod, dynamic reset PASS\n";
    return 0;
}

#include "../../../../../src/Math/MathPackage/Combinatorics/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("Combinatorics/zero", [] {
        checkPascal<Small>(0);
    });
    runCase("Combinatorics/one", [] {
        checkPascal<Small>(1);
    });
    runCase("Combinatorics/prime-boundary", [] {
        checkPascal<Small>(96);
    });
    runCase("Combinatorics/wide-modulus", [] {
        checkPascal<Large>(37);
    });
    runCase("Combinatorics/shared-grow", [] {
        auto &c = Comb<Z>::shared(150);
        CHECK(c.C(150, 0) == Z(1));
        CHECK(c.C(150, 150) == Z(1));
    });
    runCase("Combinatorics/symmetry", [] {
        Comb<Z> c(64);
        for (int k = 0; k <= 64; ++k)
            CHECK(c.C(64, k) == c.C(64, 64 - k));
    });
    runCase("Combinatorics/invalid-k", [] {
        Comb<Z> c(8);
        CHECK(c.C(8, -2) == Z(0));
        CHECK(c.C(8, 9) == Z(0));
        CHECK(c.A(-1, 0) == Z(0));
    });
    runCase("Combinatorics/dynamic-switch", [] {
        Dynamic::setMod(101);
        Comb<Dynamic> c(90);
        Dynamic::setMod(97);
        CHECK(c.C(10, 5) == Dynamic(252));
    });
    runCase("Combinatorics/permutation", [] {
        Comb<Z> c(12);
        CHECK(c.A(12, 4) == Z(11880));
    });
    runCase("Combinatorics/grow-shrink", [] {
        Comb<Z> c(1);
        c.init(80);
        auto v = c.C(80, 40);
        c.init(3);
        CHECK(c.C(80, 40) == v);
    });
    return 0;
}
}

int main() {
    runCase("Combinatorics/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
