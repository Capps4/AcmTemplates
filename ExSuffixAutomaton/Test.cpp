#include "Final.hpp"
#include "../TestSupport.hpp"
#include <set>
std::set<std::string> substrings(std::string_view text){
    std::set<std::string> result;
    for(std::size_t l=0;l<text.size();++l)for(std::size_t r=l+1;r<=text.size();++r)result.emplace(text.substr(l,r-l));
    return result;
}

template<int Z,char Base>
void verify(const std::vector<std::string>& texts){
    ExSam<Z,Base> sam(texts),online;
    std::set<std::string> expected;
    for(const auto& text:texts){auto words=substrings(text);expected.insert(words.begin(),words.end());online.addString(text);CHECK(online.distinctSubstringCount()==static_cast<long long>(expected.size()));}
    CHECK(sam.distinctSubstringCount()==static_cast<long long>(expected.size()));
    CHECK(sam.contains(""));for(const auto& word:expected)CHECK(sam.contains(word));
    CHECK(sam.son==online.son && sam.len==online.len && sam.link==online.link);
    for(int state=1;state<=sam.tot;++state)CHECK(sam.len[sam.link[state]]<sam.len[state]);
    for(int i=0;i<100;++i){std::string candidate(randomInt(1,10),Base);for(auto& c:candidate)c=char(static_cast<unsigned char>(Base)+randomInt(0,Z-1));CHECK(sam.contains(candidate)==bool(expected.count(candidate)));}
    std::vector<std::string_view> views;for(const auto& t:texts)views.emplace_back(t);CHECK((ExSam<Z,Base>(views).distinctSubstringCount()==sam.distinctSubstringCount()));
}
int main(){
    ExSam<26,'a'> literal({"ab","cd"});CHECK(literal.contains("ab") && !literal.contains("bc"));
    std::vector<std::string> words;
    for(int n=0;n<=4;++n)for(int mask=0;mask<(1<<n);++mask){std::string text(n,'a');for(int i=0;i<n;++i)text[i]+=bool(mask&(1<<i));words.push_back(text);}
    for(const auto& a:words)for(const auto& b:words)verify<2,'a'>({a,b,a});
    verify<2,'a'>({});verify<2,'a'>({"",""});verify<26,'a'>({"ab","cd"});
    ExSam<26,'a'> boundaries(std::vector<std::string>{"ab","cd"});CHECK(!boundaries.contains("bc"));
    for(int trial=0;trial<300;++trial){std::vector<std::string> text(randomInt(0,10));for(auto& t:text){t.resize(randomInt(0,20));for(auto& c:t)c=char(randomInt(0,255));}verify<256,'\0'>(text);}
    ExSam<1,'a'> adaptive;adaptive.addString(std::string(100000,'a'));adaptive.addString(std::string(200000,'a'));CHECK(adaptive.distinctSubstringCount()==200000);
    std::cout<<"ExSAM: substring-union oracle, duplicates, empty collections, boundaries, byte alphabet, append PASS\n";
}
