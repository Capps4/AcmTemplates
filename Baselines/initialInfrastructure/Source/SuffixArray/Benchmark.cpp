#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main(){
    for(int alphabet:{1,2,26}){
        std::string text(131072,'a');std::mt19937 rng(89);for(auto& c:text)c+=rng()%alphabet;
        auto checksum=[](const auto& sa){std::uint64_t result=0;for(int i=0;i<sa.n;++i)result+=std::uint64_t(sa.sa[i]+sa.rk[i]+sa.h[i])*(i+1);return result;};
        auto name="alphabet-"+std::to_string(alphabet);
        compare(name.c_str(),[&]{return checksum(Legacy::SuffixArray(text));},[&]{return checksum(SuffixArray(text));});
    }
}
