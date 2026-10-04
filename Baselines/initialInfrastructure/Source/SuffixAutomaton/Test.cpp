#include "Final.hpp"
#include "../TestSupport.hpp"
#include <set>
std::set<std::string> substrings(std::string_view text){
    std::set<std::string> result;
    for(std::size_t l=0;l<text.size();++l)for(std::size_t r=l+1;r<=text.size();++r)result.emplace(text.substr(l,r-l));
    return result;
}

template<int Z,char Base>
void verify(const std::string& text){
    Sam<Z,Base> sam(text),online;
    auto expected=substrings(text);
    CHECK(sam.distinctSubstringCount()==static_cast<long long>(expected.size()));
    CHECK(sam.contains(""));
    for(const auto& word:expected)CHECK(sam.contains(word));
    std::string prefix;
    for(std::size_t i=0;i<text.size();++i){prefix+=text[i];auto cur=online.add(int(i),text[i]);CHECK(cur==online.last);CHECK(online.distinctSubstringCount()==static_cast<long long>(substrings(prefix).size()));}
    CHECK(sam.son==online.son && sam.len==online.len && sam.link==online.link);
    for(int state=1;state<=sam.tot;++state){CHECK(sam.link[state]>=0);CHECK(sam.len[sam.link[state]]<sam.len[state]);}
    for(int i=0;i<100;++i){std::string candidate(randomInt(1,10),Base);for(auto& c:candidate)c=char(static_cast<unsigned char>(Base)+randomInt(0,Z-1));CHECK(sam.contains(candidate)==bool(expected.count(candidate)));}
}
int main(){
    for(int n=0;n<=10;++n)for(int mask=0;mask<(1<<n);++mask){std::string text(n,'a');for(int i=0;i<n;++i)text[i]+=bool(mask&(1<<i));verify<2,'a'>(text);}
    for(int trial=0;trial<300;++trial){std::string text(randomInt(0,30),'\0');for(auto& c:text)c=char(randomInt(0,255));verify<256,'\0'>(text);}
    Sam<1,'a'> undersized(1);for(int i=0;i<100000;++i)undersized.add('a');CHECK(undersized.distinctSubstringCount()==100000);
    Sam<26,'a'> owned(std::string("banana"));CHECK(owned.contains("anana"));CHECK(!owned.contains("bananab"));
    std::cout<<"SAM: exhaustive substring set, online prefixes, clone invariants, empty, bytes, adaptive capacity PASS\n";
}
