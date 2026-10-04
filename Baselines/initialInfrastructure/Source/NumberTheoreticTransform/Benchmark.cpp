#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../ModuloInteger/Original.hpp"
#include "LegacyBaseline.hpp"
}
#include "Final.hpp"
template<class T>std::uint64_t checksum(const T& values){std::uint64_t sum=0;for(std::size_t i=0;i<values.size();++i)sum+=std::uint64_t(values[i].val())*(i+1);return sum;}
int main(){
    for(int n:{64,4096,16384}){
        NttPoly a(n),b(n);Legacy::Poly oldA(n),oldB(n);std::mt19937 rng(59);
        for(int i=0;i<n;++i){int x=rng()%100,y=rng()%100;a[i]=x;oldA[i]=x;b[i]=y;oldB[i]=y;}
        a[0]=1;oldA[0]=1;
        auto name="convolution-"+std::to_string(n);
        int repeats = n == 64 ? 100 : 1;
        if(repeats > 1)name += "-100x";
        compare(name.c_str(),[&]{std::uint64_t sum=0;for(int i=0;i<repeats;++i){sum+=checksum(oldA*oldB);benchmarkConsume(sum);}return sum;},
                [&]{std::uint64_t sum=0;for(int i=0;i<repeats;++i){sum+=checksum(a*b);benchmarkConsume(sum);}return sum;});
        if(n==16384)compare("series-inverse",[&]{return checksum(oldA.inv(n));},[&]{return checksum(a.inv(n));});
    }
    NttPoly a(4096);Legacy::Poly oldA(4096);for(int i=1;i<4096;++i){a[i]=i;oldA[i]=i;}
    compare("series-exp",[&]{return checksum(oldA.exp());},[&]{return checksum(a.exp());});
    NttPoly large(1000000,7);Legacy::Poly oldLarge(1000000,7);
    compare("truncate-to-64-200x",[&]{std::uint64_t sum=0;for(int i=0;i<200;++i)sum+=checksum(oldLarge.mod(64));return sum;},
            [&]{std::uint64_t sum=0;for(int i=0;i<200;++i)sum+=checksum(large.mod(64));return sum;});
    compare("scalar-compound",[&]{auto value=oldLarge;for(int i=0;i<8;++i)value*=3;return checksum(value);},
            [&]{auto value=large;for(int i=0;i<8;++i)value*=3;return checksum(value);});
    NttPoly same(16384,3);same[0]=1;
    Legacy::Polynomial<Z> oldSame(same.begin(),same.end());
    compare("convolution-same-modint",[&]{return checksum(oldSame*oldSame);},[&]{return checksum(same*same);});
    compare("inverse-same-modint",[&]{return checksum(oldSame.inv());},[&]{return checksum(same.inv());});
}
