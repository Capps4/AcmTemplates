#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/Math/Polynomial/NumberTheoreticTransform/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <limits>
#include <sstream>

template<class T> using Series = std::vector<T>;
template<class T>
Series<T> naiveProduct(const Series<T>& a,const Series<T>& b,int count=-1){
    if(count<0)count=a.empty()||b.empty()?0:int(a.size()+b.size()-1);
    Series<T> result(count);
    for(std::size_t i=0;i<a.size() && i<std::size_t(count);++i)
        for(std::size_t j=0;j<b.size() && i+j<std::size_t(count);++j)result[i+j]+=a[i]*b[j];
    return result;
}
template<class T>
Series<T> inverse(const Series<T>& a,int m){
    Series<T> b(m);if(!m)return b;b[0]=a[0].inv();
    for(int i=1;i<m;++i){for(int j=1;j<=i && j<int(a.size());++j)b[i]-=a[j]*b[i-j];b[i]*=b[0];}
    return b;
}
template<class T>
Series<T> exponential(const Series<T>& a,int m){
    Series<T> b(m);if(!m)return b;b[0]=1;
    for(int i=1;i<m;++i){for(int j=1;j<=i && j<int(a.size());++j)b[i]+=T(j)*a[j]*b[i-j];b[i]/=i;}
    return b;
}
template<class T>
Series<T> squareRoot(const Series<T>& a,int m){
    Series<T> b(m);if(!m)return b;b[0]=1;
    for(int i=1;i<m;++i){b[i]=i<int(a.size())?a[i]:T(0);for(int j=1;j<i;++j)b[i]-=b[j]*b[i-j];b[i]/=2;}
    return b;
}
template<class T>
Series<T> power(Series<T> a,long long exponent,int m){
    a.resize(m);Series<T> result(m);if(m)result[0]=1;
    for(;exponent;exponent>>=1){if(exponent&1)result=naiveProduct(result,a,m);a=naiveProduct(a,a,m);}
    return result;
}
template<class Poly>
void verifySeries(int m){
    using T=typename Poly::value_type;
    Poly a(m),b(m);
    for(auto& x:a)x=randomInt(-50,50);
    for(auto& x:b)x=randomInt(-50,50);
    CHECK(a*b==naiveProduct<T>(a,b));
    if(!m){CHECK(a.integr()==Series<T>{0});CHECK(a.inv().empty());CHECK(a.ln().empty());CHECK(a.exp().empty());CHECK(a.power(0).empty());CHECK(a.sqrt().empty());return;}
    a[0]=randomInt(1,50);
    auto inv=a.inv(m);CHECK(inv==inverse<T>(a,m));
    CHECK(naiveProduct<T>(a,inv,m)==Poly{1}.mod(m));
    CHECK(a.integr().deriv()==a);
    a[0]=0;auto exp=a.exp(m);CHECK(exp==exponential<T>(a,m));CHECK(exp.ln(m)==a);
    a[0]=1;auto root=a.sqrt(m);CHECK(root==squareRoot<T>(a,m));CHECK(naiveProduct<T>(root,root,m)==a);
    for(long long exponent:{0LL,1LL,2LL,5LL,6LL,11LL})CHECK(a.power(exponent,m)==power<T>(a,exponent,m));
    Poly clone=a;clone+=clone;CHECK(clone==a+a);clone=a;clone-=clone;CHECK(clone==Poly(m));
    clone=a;clone*=clone;CHECK(clone==a*a);clone=a;clone*=T(3);clone/=T(3);CHECK(clone==a);
    T eval=0,term=1;for(auto x:a){eval+=x*term;term*=T(7);}CHECK(a.whenXis(7)==eval);
    CHECK(a.mulxk(5).divxk(5)==a);CHECK(a.divxk(m).empty());
}
int coreCases(){
    for(int m:{0,1,2,3,7,31,32,63,64,127,128,129,200,257})verifySeries<Poly>(m);
    for(int i=0;i<4;++i)verifySeries<Poly>(randomInt(1,100));
    for(int n:{0,1,2,4,256,1024,16}){
        Poly a(n);for(auto& x:a)x=randomInt(-50,50);auto original=a;dft(a);idft(a);CHECK(a==original);
    }
    Poly a{2,3,5};
    for(long long exponent:{998244353LL,998244354LL,std::numeric_limits<long long>::max()})
        CHECK(a.power(exponent,16)==power<Z>(a,exponent,16));
    CHECK(Poly{0,1}.power(998244354LL,8)==Poly(8));
    CHECK(Poly{2}.power(998244353LL,8)==Poly{2}.mod(8));
    CHECK(Poly{}.exp(8)==Poly{1}.mod(8));
    CHECK(Poly{}.power(0,8)==Poly{1}.mod(8));
    CHECK(Poly{}.power(9,8)==Poly(8));
    std::vector<Poly> factors{{1,2},{3,4,5},{2},{7,1}};
    Poly expected{1};for(const auto& p:factors)expected=expected*p;
    CHECK(Poly::prod(factors)==expected);CHECK(Poly::prod({})==Poly{1});
    std::stringstream io;io<<a;CHECK(io.str()=="2 3 5");Poly parsed(3);io>>parsed;CHECK(parsed==a);
    std::vector<Poly> binomialFactors(16, Poly{1, 1});
    auto binomial = Poly::prod(binomialFactors);
    std::vector<long long> pascal(17); pascal[0] = 1;
    for (int n = 0; n < 16; ++n)
        for (int k = n + 1; k > 0; --k)
            pascal[k] += pascal[k - 1];
    for (int i = 0; i <= 16; ++i) CHECK(binomial[i] == Z(pascal[i]));
    std::cout<<"NTT: naive products and series recurrences, roots, fallback, empty, large exponents, aliasing, IO PASS\n";
    return 0;
}

#include "../../../../../src/Math/Polynomial/NumberTheoreticTransform/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
void verifyAdded(const std::vector<Z> &a, const std::vector<Z> &b) {
    Poly x(a.begin(), a.end()), y(b.begin(), b.end());
    auto c = x * y;
    std::vector<Z> e(a.empty() or b.empty() ? 0 : a.size() + b.size() - 1);
    for (std::size_t i = 0; i < a.size(); ++i)
        for (std::size_t j = 0; j < b.size(); ++j)
            e[i + j] += a[i] * b[j];
    CHECK(c.size() == e.size());
    for (std::size_t i = 0; i < e.size(); ++i)
        CHECK(c[i] == e[i]);
}

int run() {
    runCase("NumberTheoreticTransform/empty-both", [] {
        verifyAdded({}, {});
    });
    runCase("NumberTheoreticTransform/empty-right", [] {
        verifyAdded({1, 2, 3}, {});
    });
    runCase("NumberTheoreticTransform/scalar", [] {
        verifyAdded({-7}, {11});
    });
    runCase("NumberTheoreticTransform/trailing-zero", [] {
        verifyAdded({1, 0, 0}, {0, 0, 2});
    });
    runCase("NumberTheoreticTransform/negative-cancellation", [] {
        verifyAdded({1, -1, 1, -1}, {1, 1, 1, 1});
    });
    runCase("NumberTheoreticTransform/small-side-fallback", [] {
        verifyAdded(std::vector<Z>(127, 1), std::vector<Z>(257, -1));
    });
    runCase("NumberTheoreticTransform/exact-threshold", [] {
        verifyAdded(std::vector<Z>(128, 1), std::vector<Z>(128, 1));
    });
    runCase("NumberTheoreticTransform/power-boundary", [] {
        verifyAdded(std::vector<Z>(129, 1), std::vector<Z>(128, 2));
    });
    runCase("NumberTheoreticTransform/impulse", [] {
        std::vector<Z> a(257), b(129);
        a[256] = 3;
        b[64] = -2;
        verifyAdded(a, b);
    });
    runCase("NumberTheoreticTransform/cache-resize", [] {
        verifyAdded(std::vector<Z>(1025, 1), std::vector<Z>(129, -1));
        verifyAdded(std::vector<Z>(128, 3), std::vector<Z>(128, 2));
    });
    return 0;
}
}

int main() {
    runCase("NumberTheoreticTransform/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
