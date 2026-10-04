#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main(){
    for(int length:{10,1000}){
        std::mt19937 rng(83);std::vector<std::string> patterns(1000,std::string(length,'a'));
        for(auto& p:patterns)for(auto& c:p)c+=rng()%26;
        auto consume=[](const auto& ac){std::uint64_t sum=0;for(int i=0;i<=ac.tot;++i)for(int v:ac.son[i])sum+=v;return sum;};
        auto name="pattern-length-"+std::to_string(length);
        compare(name.c_str(),[&]{return consume(Legacy::AcAutomaton<26,'a'>(patterns));},[&]{return consume(AcAutomaton<26,'a'>(patterns));});
    }
    std::vector<std::string> duplicates(10000,std::string(100,'a'));
    compare("duplicate-patterns",[&]{Legacy::AcAutomaton<26,'a'> ac(duplicates);return std::uint64_t(ac.tot);},[&]{AcAutomaton<26,'a'> ac(duplicates);return std::uint64_t(ac.tot);});
}
