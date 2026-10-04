#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "CurrentSource.hpp"
}
#undef call
#include "Final.hpp"
template<class T>std::uint64_t checksum(const T& values){std::uint64_t result=0;for(std::size_t i=0;i<values.size();++i)result+=std::uint64_t(values[i])*(i+1);return result;}
struct Compare {std::vector<int> rank=std::vector<int>(4096);Compare(){std::iota(rank.begin(),rank.end(),0);}bool operator()(int x,int y)const{return rank[x]<rank[y];}};
int main(){
    std::mt19937_64 rng(103);std::vector<long long> data(200000);for(auto& x:data)x=static_cast<long long>(rng());
    compare("radix-sort",[&]{return checksum(data|Legacy::seq::sorted());},[&]{return checksum(data|sorted());});
    compare("radix-sort-descending",[&]{return checksum(data|Legacy::seq::sorted(std::greater<>{}));},[&]{return checksum(data|sorted(std::greater<>{}));});
    auto uniformDigits=data;for(auto& x:uniformDigits)x=static_cast<long long>(rng()%256 | ((rng()%256)<<40));
    compare("radix-uniform-middle-digits",[&]{return checksum(uniformDigits|Legacy::seq::sorted());},[&]{return checksum(uniformDigits|sorted());});
    compare("slice-lvalue-5000x",[&]{std::uint64_t sum=0;for(int i=0;i<5000;++i){sum+=checksum(data|Legacy::seq::slice(100,1000));benchmarkConsume(sum);}return sum;},
            [&]{std::uint64_t sum=0;for(int i=0;i<5000;++i){sum+=checksum(data|slice(100,1000));benchmarkConsume(sum);}return sum;});
    auto before=Legacy::seq::sorted(Compare{});auto after=sorted(Compare{});std::vector<int> small(32);for(auto& x:small)x=rng()%4096;
    compare("cached-heavy-comparator-5000x",[&]{std::uint64_t sum=0;for(int i=0;i<5000;++i)sum+=checksum(small|before);return sum;},[&]{std::uint64_t sum=0;for(int i=0;i<5000;++i)sum+=checksum(small|after);return sum;});
}
