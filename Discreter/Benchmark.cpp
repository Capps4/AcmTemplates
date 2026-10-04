#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../ListHelper/Original.hpp"
#include "Original.hpp"
}
#undef call
#include "Final.hpp"
std::uint64_t checksum(const std::vector<int>& ranks){std::uint64_t result=0;for(std::size_t i=0;i<ranks.size();++i)result+=std::uint64_t(ranks[i])*(i+1);return result;}
int main(){
    std::vector<int> basis(200000);std::iota(basis.begin(),basis.end(),0);std::mt19937 rng(109);std::vector<int> query(200000);for(auto& x:query)x=rng()%300000;
    auto before=Legacy::discreteFrom(basis);auto after=discreteFrom(basis);compare("borrowed-basis",[&]{return checksum(query|before);},[&]{return checksum(query|after);});
    std::vector<int> shortQuery(query.begin(),query.begin()+1000);auto beforeOwned=Legacy::discreteFrom(std::vector<int>(basis));auto afterOwned=discreteFrom(std::vector<int>(basis));
    compare("cached-owned-basis-100x",[&]{std::uint64_t value=0;for(int i=0;i<100;++i)value+=checksum(shortQuery|beforeOwned);return value;},[&]{std::uint64_t value=0;for(int i=0;i<100;++i)value+=checksum(shortQuery|afterOwned);return value;});
}
