#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main(){
    for(int alphabet:{1,2,26}){
        std::mt19937 rng(79);std::vector<std::string> text(1000,std::string(100,'a'));for(auto& t:text)for(auto& c:t)c+=rng()%alphabet;
        auto consume=[](const auto& sam){std::uint64_t value=0;for(unsigned state=1;state<=sam.tot;++state)value+=std::uint64_t(sam.len[state]-sam.len[sam.link[state]]);return value;};
        auto name="build-alphabet-"+std::to_string(alphabet);
        compare(name.c_str(),[&]{return consume(Legacy::ExSam<26,'a'>(text));},[&]{return consume(ExSam<26,'a'>(text));});
    }
}
