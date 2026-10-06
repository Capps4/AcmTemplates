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
int main() {
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
}
