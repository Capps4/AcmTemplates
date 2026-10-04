#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main(){
    const int n=200000;std::mt19937 rng(101);std::vector<std::pair<int,int>> edges(400000);for(auto& [x,y]:edges){x=rng()%n;y=rng()%n;}
    auto randomRun=[&](auto d){for(auto [x,y]:edges)d.Union(x,y);std::uint64_t sum=0;for(int i=0;i<n;++i)sum+=d.size[d.find(i)];return sum;};
    compare("random-merge-query",[&]{return randomRun(Legacy::DSU(n));},[&]{return randomRun(DSU(n));});
    auto orderedRun=[&](auto d){for(int i=1;i<n;++i)d.Union(i,i-1);std::uint64_t sum=0;for(int i=0;i<n;++i)sum+=d.size[d.find(i)];return sum;};
    compare("ordered-merge-query",[&]{return orderedRun(Legacy::DSU(n));},[&]{return orderedRun(DSU(n));});
}
