#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main(){
    for(int alphabet:{1,2,26}){
        std::string text(1000000,'a');std::mt19937 rng(73);for(auto& x:text)x+=rng()%alphabet;
        auto name="alphabet-"+std::to_string(alphabet);
        auto consume=[](const auto& engine){std::uint64_t sum=0;for(int i=0;i<1000000;++i)sum+=engine.getPalinLenFromTail(i)+engine.getPalinLenFromCenter(i,0);return sum;};
        compare(name.c_str(),[&]{return consume(Legacy::Manacher(text));},[&]{return consume(Manacher(text));});
    }
    std::string text(2000000,'a');auto view=std::string_view(text).substr(1,1000000);
    compare("substring-view",[&]{Legacy::Manacher engine{std::string(view)};return std::uint64_t(engine.getPalinLenFromTail(999999));},
            [&]{Manacher engine(view);return std::uint64_t(engine.getPalinLenFromTail(999999));});
}
