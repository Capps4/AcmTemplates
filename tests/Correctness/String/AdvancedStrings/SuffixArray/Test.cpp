#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/String/AdvancedStrings/SuffixArray/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>

template<class Seq,class Cmp>
void verify(const Seq& s,Cmp cmp){
    SuffixArray suffixes(s);int n=int(s.size());
    std::vector<int> expected(n);std::iota(expected.begin(),expected.end(),0);
    auto equiv=[&](int x,int y){return !cmp(s[x],s[y]) && !cmp(s[y],s[x]);};
    std::sort(expected.begin(),expected.end(),[&](int x,int y){while(x<n && y<n && equiv(x,y)){++x;++y;}return (x==n && y!=n) || (x!=n && y!=n && cmp(s[x],s[y]));});
    CHECK(suffixes.sa==expected);CHECK(suffixes.h.size()==s.size());
    for(int rank=0;rank<n;++rank){CHECK(suffixes.rk[expected[rank]]==rank);int length=0;if(rank)while(expected[rank]+length<n && expected[rank-1]+length<n && equiv(expected[rank]+length,expected[rank-1]+length))++length;CHECK(suffixes.h[rank]==length);}
}
void verifyBytes(const std::string& text){
    std::vector<unsigned char> bytes(text.begin(),text.end());verify(bytes,std::less<>{});
    SuffixArray actual(text),expected(bytes);CHECK(actual.sa==expected.sa && actual.rk==expected.rk && actual.h==expected.h);
    verify(std::string_view(text),[](char a,char b){return static_cast<unsigned char>(a)<static_cast<unsigned char>(b);});
}
struct Record {int key;int ignored; bool operator<(Record b) const {return key < b.key;} bool operator==(Record b) const {return key == b.key;} };
int coreCases(){
    for(int n=0;n<=11;++n)for(int mask=0;mask<(1<<n);++mask){std::string s(n,'a');for(int i=0;i<n;++i)s[i]+=bool(mask&(1<<i));verifyBytes(s);}
    for(int test=0;test<16;++test){
        test_context::step = test;std::string text(randomInt(0,100),'\0');for(auto& c:text)c=char(randomInt(0,255));verifyBytes(text);std::vector<long long> numeric(text.size());for(auto& x:numeric)x=static_cast<long long>(testRng());verify(numeric,std::less<>{});}
    verify(std::vector<long long>{std::numeric_limits<long long>::min(),0,-1,std::numeric_limits<long long>::max(),-1,0},std::less<>{});
    std::vector<Record> records{{1,7},{2,9},{1,8},{0,3},{1,99}};
    verify(records,[](const Record& a,const Record& b){return a.key<b.key;});
    SuffixArray literal("banana");CHECK(literal.sa==std::vector<int>({5,3,1,0,4,2}));CHECK(literal.h==std::vector<int>({0,1,3,0,0,2}));
    std::string repeated(128,'x');SuffixArray large(repeated);for(int i=0;i<128;++i){CHECK(large.sa[i]==127-i);CHECK(large.h[i]==i);}
    std::cout<<"SuffixArray: naive suffix ordering/LCP, exhaustive binary, bytes, signed extremes, value equality, empty PASS\n";
    return 0;
}

#include "../../../../../src/String/AdvancedStrings/SuffixArray/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
template <class Seq, class Cmp>
void verify(const Seq &s, Cmp cmp) {
    SuffixArray suffixes(s);
    int n = int(s.size());
    std::vector<int> expected(n);
    std::iota(expected.begin(), expected.end(), 0);
    auto equiv = [&](int x, int y) {
        return !cmp(s[x], s[y]) && !cmp(s[y], s[x]);
    };
    std::sort(expected.begin(), expected.end(), [&](int x, int y) {
        while (x < n && y < n && equiv(x, y)) {
            ++x;
            ++y;
        }
        return (x == n && y != n) || (x != n && y != n && cmp(s[x], s[y]));
    });
    CHECK(suffixes.sa == expected);
    CHECK(suffixes.h.size() == s.size());
    for (int rank = 0; rank < n; ++rank) {
        CHECK(suffixes.rk[expected[rank]] == rank);
        int length = 0;
        if (rank)
            while (expected[rank] + length < n && expected[rank - 1] + length < n &&
                   equiv(expected[rank] + length, expected[rank - 1] + length))
                ++length;
        CHECK(suffixes.h[rank] == length);
    }
}
void verifyBytes(const std::string &text) {
    std::vector<unsigned char> bytes(text.begin(), text.end());
    verify(bytes, std::less<>{});
    SuffixArray actual(text), expected(bytes);
    CHECK(actual.sa == expected.sa && actual.rk == expected.rk && actual.h == expected.h);
    verify(std::string_view(text), [](char a, char b) {
        return static_cast<unsigned char>(a) < static_cast<unsigned char>(b);
    });
}
struct Record {
    int key;
    int ignored;
    bool operator<(Record b) const {
        return key < b.key;
    }
    bool operator==(Record b) const {
        return key == b.key;
    }
};

int run() {
    runCase("SuffixArray/empty", [] {
        verifyBytes("");
    });
    runCase("SuffixArray/single", [] {
        verifyBytes("a");
    });
    runCase("SuffixArray/overlap", [] {
        verifyBytes("aaabaaa");
    });
    runCase("SuffixArray/periodic-tail", [] {
        verifyBytes("abcabcab");
    });
    runCase("SuffixArray/alphabet", [] {
        verifyBytes("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("SuffixArray/even-palindromes", [] {
        verifyBytes("abbaabba");
    });
    runCase("SuffixArray/byte-domain", [] {
        verifyBytes(std::string("\0\xff\x80\0", 4));
    });
    return 0;
}
}

int main() {
    runCase("SuffixArray/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
