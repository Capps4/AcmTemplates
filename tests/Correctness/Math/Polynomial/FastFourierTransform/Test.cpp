#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/Polynomial/FastFourierTransform/code.hpp"
#include "../../../../Support/TestSupport.hpp"

template<class T>
void verify(int n, int m) {
    _fft::Polynomial<T,std::complex> a(n), b(m);
    for(auto& x:a)x=T(randomInt(-100,100))/7;
    for(auto& x:b)x=T(randomInt(-100,100))/11;
    auto product=a*b;
    if(n==0 || m==0){CHECK(product.empty());return;}
    std::vector<long double> expected(n+m-1);
    for(int i=0;i<n;++i)for(int j=0;j<m;++j)expected[i+j]+=static_cast<long double>(a[i])*b[j];
    CHECK(product.size()==expected.size());
    for(std::size_t i=0;i<expected.size();++i)
        CHECK(std::abs(static_cast<long double>(product[i])-expected[i])<1e-8L+1e-10L*std::abs(expected[i]));
}
int coreCases(){
    for(int n:{0,1,2,3,63,127,128,129,255,256,257,1024})
        for(int m:{0,1,31,128,129,259}) {verify<double>(n,m);verify<long double>(n,m);}
    for(int i=0;i<4;++i)verify<double>(randomInt(1,129),randomInt(1,129));
    Poly ones(256,1);
    auto triangle=ones*ones;
    for(int i=0;i<511;++i) CHECK(std::abs(triangle[i]-std::min(i+1,511-i))<1e-6);
    Poly impulse(129); impulse[64]=1;
    auto shifted=ones*impulse;
    for(int i=0;i<384;++i) CHECK(std::abs(shifted[i]-(i>=64 && i<320 ? 1:0))<1e-9);
    CHECK((Poly{2}*Poly{3})[0]==6);
    std::cout<<"FFT: naive long-double oracle, empty, threshold, alternating sizes, impulse, long-double PASS\n";
    return 0;
}

#include "../../../../../src/Math/Polynomial/FastFourierTransform/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(const std::vector<double> &a, const std::vector<double> &b) {
    test_context::describe(a);
    Poly x(a.begin(), a.end()), y(b.begin(), b.end());
    auto c = x * y;
    std::vector<double> e(a.empty() or b.empty() ? 0 : a.size() + b.size() - 1);
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j)
            e[i + j] += a[i] * b[j];
    CHECK(c.size() == e.size());
    for (std::size_t i = 0; i < e.size(); ++i)
        CHECK(std::abs(c[i] - e[i]) < 1e-7 * (1 + std::abs(e[i])));
}

int run() {
    runCase("FastFourierTransform/empty-both", [] {
        verifyAdded({}, {});
    });
    runCase("FastFourierTransform/empty-right", [] {
        verifyAdded({1, 2, 3}, {});
    });
    runCase("FastFourierTransform/scalar", [] {
        verifyAdded({-7}, {11});
    });
    runCase("FastFourierTransform/trailing-zero", [] {
        verifyAdded({1, 0, 0}, {0, 0, 2});
    });
    runCase("FastFourierTransform/negative-cancellation", [] {
        verifyAdded({1, -1, 1, -1}, {1, 1, 1, 1});
    });
    runCase("FastFourierTransform/small-side-fallback", [] {
        verifyAdded(std::vector<double>(127, 1), std::vector<double>(257, -1));
    });
    runCase("FastFourierTransform/exact-threshold", [] {
        verifyAdded(std::vector<double>(128, 1), std::vector<double>(128, 1));
    });
    runCase("FastFourierTransform/power-boundary", [] {
        verifyAdded(std::vector<double>(129, 1), std::vector<double>(128, 2));
    });
    runCase("FastFourierTransform/impulse", [] {
        std::vector<double> a(257), b(129);
        a[256] = 3;
        b[64] = -2;
        verifyAdded(a, b);
    });
    runCase("FastFourierTransform/cache-resize", [] {
        verifyAdded(std::vector<double>(1025, 1), std::vector<double>(129, -1));
        verifyAdded(std::vector<double>(128, 3), std::vector<double>(128, 2));
    });
    return 0;
}
}

int main() {
    runCase("FastFourierTransform/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
