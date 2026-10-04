#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../ListHelper/Original.hpp"
#include "Original.hpp"
}
#undef call
#include "Final.hpp"
template<class Seq>std::uint64_t checksum(const Seq& sequence){std::uint64_t value=0;for(std::size_t i=0;i<sequence.size();++i)value+=std::uint64_t(sequence[i])*(i+1);return value;}
int main(){
    for(int alphabet:{1,2,26}){std::string text(500000,'a');std::mt19937 rng(107);for(auto& c:text)c+=rng()%alphabet;auto name="alphabet-"+std::to_string(alphabet);compare(name.c_str(),[&]{return checksum(text|Legacy::minimalString());},[&]{return checksum(text|minimalString());});}
}
