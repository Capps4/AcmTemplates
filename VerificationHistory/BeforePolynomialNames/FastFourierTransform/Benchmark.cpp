#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
template<class T>
std::uint64_t checksum(const T& values){
    std::uint64_t sum=0;
    for(std::size_t i=0;i<values.size();++i)sum+=std::uint64_t(std::llround(values[i]))*(i+1);
    return sum;
}
int main(){
    for(int n:{64,4096,16384}){
        Poly a(n),b(n);Legacy::Poly oldA(n),oldB(n);
        std::mt19937 rng(51);
        for(int i=0;i<n;++i){a[i]=oldA[i]=rng()%20;b[i]=oldB[i]=rng()%20;}
        auto label="equal-"+std::to_string(n);
        int repeats = n == 64 ? 100 : 1;
        if(repeats > 1)label += "-100x";
        compare(label.c_str(),[&]{std::uint64_t sum=0;for(int i=0;i<repeats;++i){sum+=checksum(oldA*oldB);benchmarkConsume(sum);}return sum;},
                [&]{std::uint64_t sum=0;for(int i=0;i<repeats;++i){sum+=checksum(a*b);benchmarkConsume(sum);}return sum;});
    }
    Poly a(4096,1),b(8192,2);Legacy::Poly oldA(4096,1),oldB(8192,2);
    compare("alternating-cold-cache",[&]{return checksum(oldA*oldA)+checksum(oldB*oldB);},
            [&]{return checksum(a*a)+checksum(b*b);});
}
