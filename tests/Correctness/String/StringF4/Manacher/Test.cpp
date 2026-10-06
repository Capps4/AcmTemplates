#include "../../../../../src/String/StringF4/Manacher/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>
bool palindrome(std::string_view text,int l,int r){while(l<--r)if(text[l++]!=text[r])return false;return true;}
void verify(std::string_view text){
    Manacher engine(text);int n=int(text.size());
    for(int i=0;i<n;++i){
        int odd=1,even=0,longest=0;
        for(int k=1;k<=i && i+k<n && text[i-k]==text[i+k];++k)odd+=2;
        if(i+1<n){for(int k=0;k<=i && i+k+1<n && text[i-k]==text[i+k+1];++k)even+=2;CHECK(engine.getPalinLenFromCenter(i,1)==even);}
        CHECK(engine.getPalinLenFromCenter(i,0)==odd);
        for(int l=0;l<=i;++l)if(palindrome(text,l,i+1))longest=std::max(longest,i+1-l);
        CHECK(engine.getPalinLenFromTail(i)==longest);
    }
    for(int l=0;l<=n;++l)for(int r=l;r<=n;++r)CHECK(engine.isPalindrome(l,r)==palindrome(text,l,r));
}
int main(){
    for(int n=0;n<=11;++n)for(int mask=0;mask<(1<<n);++mask){std::string s(n,'a');for(int i=0;i<n;++i)s[i]+=bool(mask&(1<<i));verify(s);}
    for(std::string text:{"[", "]", "-", "[-]", "[]]", "[a[", "-a-", "abba", "abacaba"})verify(text);
    std::string bytes;for(int i=0;i<256;++i)bytes.push_back(char(i));verify(bytes);
    for(int trial=0;trial<1000;++trial){std::string text(randomInt(0,100),'\0');for(auto& x:text)x=char(randomInt(0,255));verify(text);if(text.size()>2)verify(std::string_view(text).substr(1,text.size()-2));}
    std::string repeated(1000000,'x');Manacher engine(repeated);
    for(int i=0;i<1000000;++i){CHECK(engine.getPalinLenFromTail(i)==i+1);CHECK(engine.getPalinLenFromCenter(i)==2*std::min(i,999999-i)+1);}
    CHECK(engine.isPalindrome(0,1000000));
    Manacher owned(std::string("abba"));CHECK(owned.isPalindrome(0,4));
    std::cout<<"Manacher: exhaustive binary, sentinel bytes, all intervals, naive center/tail, million bytes PASS\n";
}
