#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main(){
    for(int alphabet:{1,2,26}){
        std::mt19937 rng(79);std::string text(100000,'a');for(auto& c:text)c+=rng()%alphabet;
        auto consume=[](const auto& sam){std::uint64_t value=0;for(unsigned state=2;state<=sam.tot;++state)value+=sam.len[state]+sam.link[state];return value;};
        auto name="build-alphabet-"+std::to_string(alphabet);
        compare(name.c_str(),[&]{return consume(Legacy::Pam<26,'a'>(text));},[&]{return consume(Pam<26,'a'>(text));});
    }
}
