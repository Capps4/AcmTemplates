#include "../../../../../src/Math/Polynomial/NumberTheoreticTransform/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../../src/Math/MathPackage/Combinatorics/code.hpp"
#include "../../../../../src/Math/MathPackage/Sieve/code.hpp"
#include "../../../../../src/Math/RandomNumberAlgorithm/PollardRho/code.hpp"
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
int main(){
    for(int m:{0,1,2,3,7,31,32,63,64,127,128,129,200,257})verifySeries<Poly>(m);
    for(int i=0;i<40;++i)verifySeries<Poly>(randomInt(1,100));
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
    // Cross-module math integration: product tree, formal powers, binomial cache,
    // sieve, primality and rho coexist with a separate runtime modulus.
    ModuloInteger<long long,0>::setMod(101);
    std::vector<Poly> binomialFactors(200,Poly{1,1});
    auto binomial=Poly::prod(binomialFactors);
    CHECK(binomial==Poly{1,1}.power(200,201));
    for(int i=0;i<=200;++i)CHECK(binomial[i]==comb.C(200,i));
    PollardRho<long long> factorizer(17);
    auto primeFactors=factorizer.primeFactorize(1000000016000000063LL);
    for(auto [p,e]:primeFactors)CHECK(isPrime(p) && e==1);
    CHECK(siv.primeFactorize(123456LL)==factorizer.primeFactorize(123456LL));
    CHECK((ModuloInteger<long long,0>::getMod()==101));
    std::cout<<"NTT: naive products and series recurrences, roots, fallback, empty, large exponents, aliasing, IO PASS\n";
}
