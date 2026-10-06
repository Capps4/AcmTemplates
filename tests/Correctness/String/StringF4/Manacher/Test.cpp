#include "../../../../Support/CaseSupport.hpp"
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
int coreCases(){
    for(int n=0;n<=11;++n)for(int mask=0;mask<(1<<n);++mask){std::string s(n,'a');for(int i=0;i<n;++i)s[i]+=bool(mask&(1<<i));verify(s);}
    for(std::string text:{"[", "]", "-", "[-]", "[]]", "[a[", "-a-", "abba", "abacaba"})verify(text);
    std::string bytes;for(int i=0;i<256;++i)bytes.push_back(char(i));verify(bytes);
    for(int trial=0;trial<16;++trial){
        test_context::step = trial;std::string text(randomInt(0,100),'\0');for(auto& x:text)x=char(randomInt(0,255));verify(text);if(text.size()>2)verify(std::string_view(text).substr(1,text.size()-2));}
    std::string repeated(128,'x');Manacher engine(repeated);
    for(int i=0;i<128;++i){CHECK(engine.getPalinLenFromTail(i)==i+1);CHECK(engine.getPalinLenFromCenter(i)==2*std::min(i,127-i)+1);}
    CHECK(engine.isPalindrome(0,128));
    Manacher owned(std::string("abba"));CHECK(owned.isPalindrome(0,4));
    std::cout<<"Manacher: exhaustive binary, sentinel bytes, all intervals, naive center/tail, million bytes PASS\n";
    return 0;
}

#include "../../../../../src/String/StringF4/Manacher/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
bool palindrome(std::string_view text, int l, int r) {
    while (l < --r)
        if (text[l++] != text[r])
            return false;
    return true;
}
void verify(std::string_view text) {
    Manacher engine(text);
    int n = int(text.size());
    for (int i = 0; i < n; ++i) {
        int odd = 1, even = 0, longest = 0;
        for (int k = 1; k <= i && i + k < n && text[i - k] == text[i + k]; ++k)
            odd += 2;
        if (i + 1 < n) {
            for (int k = 0; k <= i && i + k + 1 < n && text[i - k] == text[i + k + 1]; ++k)
                even += 2;
            CHECK(engine.getPalinLenFromCenter(i, 1) == even);
        }
        CHECK(engine.getPalinLenFromCenter(i, 0) == odd);
        for (int l = 0; l <= i; ++l)
            if (palindrome(text, l, i + 1))
                longest = std::max(longest, i + 1 - l);
        CHECK(engine.getPalinLenFromTail(i) == longest);
    }
    for (int l = 0; l <= n; ++l)
        for (int r = l; r <= n; ++r)
            CHECK(engine.isPalindrome(l, r) == palindrome(text, l, r));
}

int run() {
    runCase("Manacher/empty", [] {
        verify("");
    });
    runCase("Manacher/single", [] {
        verify("a");
    });
    runCase("Manacher/overlap", [] {
        verify("aaabaaa");
    });
    runCase("Manacher/periodic-tail", [] {
        verify("abcabcab");
    });
    runCase("Manacher/nested-palindromes", [] {
        verify("abacabadabacaba");
    });
    runCase("Manacher/odd-palindrome", [] {
        verify("abcdefedcba");
    });
    runCase("Manacher/even-palindromes", [] {
        verify("abbaabba");
    });
    runCase("Manacher/byte-domain", [] {
        verify(std::string("\0\xff\x80\0", 4));
    });
    return 0;
}
}

int main() {
    runCase("Manacher/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
