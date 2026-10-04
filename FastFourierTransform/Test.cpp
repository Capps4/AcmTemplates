#include "Final.hpp"
#include "../TestSupport.hpp"
template<class T>
void verify(int n, int m) {
    FftPolynomial<T,std::complex> a(n), b(m);
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
int main(){
    for(int n:{0,1,2,3,63,127,128,129,255,256,257,1024})
        for(int m:{0,1,31,128,129,259}) {verify<double>(n,m);verify<long double>(n,m);}
    for(int i=0;i<100;++i)verify<double>(randomInt(1,500),randomInt(1,500));
    FftPoly ones(10000,1);
    auto triangle=ones*ones;
    for(int i=0;i<19999;++i) CHECK(std::abs(triangle[i]-std::min(i+1,19999-i))<1e-6);
    FftPoly impulse(129); impulse[64]=1;
    auto shifted=ones*impulse;
    for(int i=0;i<10128;++i) CHECK(std::abs(shifted[i]-(i>=64 && i<10064 ? 1:0))<1e-9);
    CHECK((FftPoly{2}*FftPoly{3})[0]==6);
    std::cout<<"FFT: naive long-double oracle, empty, threshold, alternating sizes, impulse, long-double PASS\n";
}
