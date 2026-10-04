#include "Final.hpp"
#include "../TestSupport.hpp"
#include <map>
bool palindrome(std::string_view s){for(std::size_t i=0;i<s.size()/2;++i)if(s[i]!=s[s.size()-1-i])return false;return true;}
template<int Z,char Base>
void verify(const std::string& text){
    Pam<Z,Base> pam(text),online;
    std::map<std::string,long long> expected;
    for(std::size_t l=0;l<text.size();++l)for(std::size_t r=l+1;r<=text.size();++r){auto word=text.substr(l,r-l);if(palindrome(word))++expected[word];}
    std::map<int,std::string> nodeText;
    std::string prefix;
    for(std::size_t i=0;i<text.size();++i){prefix+=text[i];int state=online.add(int(i),text[i]);auto longest=prefix.substr(prefix.size()-online.len[state]);nodeText[state]=longest;
        int count=0;for(std::size_t l=0;l<prefix.size();++l)count+=palindrome(std::string_view(prefix).substr(l));CHECK(online.dep[state]==count);}
    CHECK(pam.s==text && pam.son==online.son && pam.cnt==online.cnt && pam.link==online.link);
    CHECK(pam.tot-1==int(expected.size()));auto counts=pam.occurrences();CHECK(pam.occurrences()==counts);
    for(int state=2;state<=pam.tot;++state){CHECK(pam.len[pam.link[state]]<pam.len[state]);CHECK(counts[state]==expected.at(nodeText.at(state)));}
    auto tree=pam.getLinkTree();std::vector<int> inDegree(pam.tot+1);for(int p=0;p<=pam.tot;++p)for(int child:tree[p]){CHECK(pam.link[child]==p);++inDegree[child];}
    for(int state=0;state<=pam.tot;++state)CHECK(inDegree[state]==(state==1?0:1));
}
int main(){
    for(int n=0;n<=11;++n)for(int mask=0;mask<(1<<n);++mask){std::string text(n,'a');for(int i=0;i<n;++i)text[i]+=bool(mask&(1<<i));verify<2,'a'>(text);}
    for(int trial=0;trial<1000;++trial){std::string text(randomInt(0,40),'\0');for(auto& c:text)c=char(randomInt(0,255));verify<256,'\0'>(text);}
    Pam<1,'a'> adaptive(1);for(int i=0;i<100000;++i)adaptive.add('a');auto counts=adaptive.occurrences();for(int state=2;state<=adaptive.tot;++state)CHECK(counts[state]==100001-adaptive.len[state]);
    Pam<26,'a'> owned(std::string("abba"));CHECK(owned.len[owned.cur]==4);
    std::cout<<"PAM: exhaustive palindromes, naive frequencies/suffix depth, roots, bytes, empty, append PASS\n";
}
