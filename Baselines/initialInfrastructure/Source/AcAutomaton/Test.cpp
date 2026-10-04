#include "Final.hpp"
#include "../TestSupport.hpp"
long long naive(std::string_view pattern,std::string_view text){
    long long result=0;
    for(std::size_t i=0;i+pattern.size()<=text.size();++i)result+=text.substr(i,pattern.size())==pattern;
    return result;
}
std::vector<std::string> words(int n){
    std::vector<std::string> result;
    for(int length=0;length<=n;++length)for(int mask=0;mask<(1<<length);++mask){std::string s(length,'a');for(int i=0;i<length;++i)s[i]+=bool(mask&(1<<i));result.push_back(s);}
    return result;
}
template<int Z,char Base>
void verify(const std::vector<std::string>& patterns,const std::string& text){
    AcAutomaton<Z,Base> ac(patterns);
    auto actual=ac.countOccurrences(text);
    CHECK(actual.size()==patterns.size());CHECK(ac.son.size()==std::size_t(ac.tot+1));CHECK(ac.order.size()==ac.son.size());
    for(std::size_t i=0;i<patterns.size();++i)CHECK(actual[i]==naive(patterns[i],text));
    CHECK(ac.countOccurrences(text)==actual);
    std::vector<std::string_view> views;for(const auto& p:patterns)views.emplace_back(p);
    CHECK((AcAutomaton<Z,Base>(views).countOccurrences(text)==actual));
}
int main(){
    AcAutomaton<26,'a'> literal({"he","she",""});CHECK(literal.countOccurrences("she")==std::vector<long long>({1,1,4}));
    auto patterns=words(3),texts=words(5);
    for(const auto& a:patterns)for(const auto& b:patterns)for(const auto& text:texts)verify<2,'a'>({a,b,a},text);
    verify<26,'a'>({},"abc");verify<26,'a'>({"","","a","aba","ba"},"ababa");
    for(int trial=0;trial<1000;++trial){
        std::vector<std::string> p(randomInt(0,20));std::string text(randomInt(0,150),'\0');
        for(auto& word:p){word.resize(randomInt(0,15));for(auto& c:word)c=char(randomInt(0,255));}
        for(auto& c:text)c=char(randomInt(0,255));
        verify<256,'\0'>(p,text);
    }
    AcAutomaton<1,'a'> repeated(std::vector<std::string>{std::string(100000,'a'),"a",""});
    CHECK(repeated.countOccurrences(std::string(1000000,'a'))==std::vector<long long>({900001,1000000,1000001}));
    std::cout<<"AC: exhaustive patterns/text, naive overlapping counts, duplicates, empty, 256 bytes, views PASS\n";
}
