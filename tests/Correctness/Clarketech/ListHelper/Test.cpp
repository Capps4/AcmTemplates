#include "../../../Support/CaseSupport.hpp"
#include "../../../../src/Clarketech/ListHelper/code.hpp"
#include "../../../Support/TestSupport.hpp"
#include <list>
#include <memory>
#include <sstream>
#include <deque>
#include <climits>

struct Box {int value;void set(int x){value=x;}int get()const{return value;}};
struct Copies {
    inline static int count=0;int value;
    Copies(int v=0):value(v){}
    Copies(const Copies& rhs):value(rhs.value){++count;}
    Copies(Copies&&)=default;Copies& operator=(const Copies&)=default;Copies& operator=(Copies&&)=default;
};
int coreCases(){
    for(int trial=0;trial<16;++trial){
        test_context::step = trial;
        std::vector<long long> a(randomInt(0,64));for(auto& x:a)x=static_cast<long long>(testRng());
        auto expected=a;std::sort(expected.begin(),expected.end());CHECK((a|sorted())==expected);CHECK(a.size()==expected.size());
        auto uniqueExpected=expected;uniqueExpected.erase(std::unique(uniqueExpected.begin(),uniqueExpected.end()),uniqueExpected.end());CHECK((a|sorted()|unique())==uniqueExpected);
        auto filtered=a;filtered.erase(std::remove_if(filtered.begin(),filtered.end(),[](auto x){return x%2!=0;}),filtered.end());CHECK((a|filter([](auto x){return x%2==0;}))==filtered);
    }
    std::vector<long long> limits{LLONG_MIN,LLONG_MAX,-1,0,1,LLONG_MIN,LLONG_MAX};auto expected=limits;std::sort(expected.begin(),expected.end());CHECK((limits|sorted())==expected);
    for(int bits:{1,7,8,16}){auto v=limits;switch(bits){case 1:seq::radixSort<1>(v,LLONG_MIN,LLONG_MAX);break;case 7:seq::radixSort<7>(v,LLONG_MIN,LLONG_MAX);break;case 8:seq::radixSort<8>(v,LLONG_MIN,LLONG_MAX);break;case 16:seq::radixSort<16>(v,LLONG_MIN,LLONG_MAX);break;}CHECK(v==expected);}
    std::vector<int> emptyRadix;
    seq::radixSort<8>(emptyRadix, 0, 100);
    for (int trial = 0; trial < 4; ++trial) {
        test_context::step = trial;
        std::vector<long long> values(5000);
        for (auto& value : values) {
            value = trial % 2 ? static_cast<long long>(testRng()) :
                static_cast<long long>((testRng() % 256) | ((testRng() % 256) << 40));
        }
        auto ascending = values; std::sort(ascending.begin(), ascending.end());
        CHECK((values | sorted(std::less<long long>{})) == ascending);
        std::deque<long long> deque(values.begin(), values.end());
        auto sortedDeque = deque | sorted();
        CHECK(std::equal(sortedDeque.begin(), sortedDeque.end(), ascending.begin()));
        std::reverse(ascending.begin(), ascending.end());
        CHECK((values | sorted(std::greater<>{})) == ascending);
        CHECK((values | sorted(std::greater<long long>{})) == ascending);
    }
    std::string bytes(5000, '\0'); for (auto& value : bytes) value = char(testRng());
    auto sortedBytes = bytes; std::sort(sortedBytes.begin(), sortedBytes.end());
    CHECK((bytes | sorted()) == sortedBytes);
    std::reverse(sortedBytes.begin(), sortedBytes.end());
    CHECK((std::string(bytes) | sorted(std::greater<char>{})) == sortedBytes);
    std::vector<int> a{3,1,2,1};auto original=a;
    auto moveOnlySort=sorted([p=std::make_unique<int>(1)](int x,int y){return *p*x<*p*y;});CHECK((a|moveOnlySort)==std::vector<int>({1,1,2,3}));CHECK((a|moveOnlySort)==std::vector<int>({1,1,2,3}));
    auto reusable=accumulate(std::string("prefix"),[](std::string result,int x){return result+std::to_string(x);});CHECK((a|reusable)=="prefix3121");CHECK((a|reusable)=="prefix3121");
    auto mutableMap=map([count=0](int)mutable{return ++count;});CHECK((a|mutableMap)==std::vector<int>({1,2,3,4}));CHECK((a|mutableMap)==std::vector<int>({5,6,7,8}));
    const auto constSort=sorted();CHECK((a|constSort)==std::vector<int>({1,1,2,3}));CHECK(a==original);
    std::string text="cbbaa";std::string_view view(text);CHECK((std::string(view)|sorted()|unique())=="abc");CHECK((std::string(view)|reverse())=="aabbc");CHECK((std::string(view)|slice(1,4))=="bba");CHECK((std::string(view)|filter([](char c){return c!='b';}))=="caa");CHECK((std::string{}|slice()).empty());CHECK((std::string(view)|slice(5,1)).empty());
    for(std::size_t l=0;l<8;++l)for(std::size_t r=0;r<8;++r){auto lo=std::min(l,a.size()),hi=std::max(lo,std::min(r,a.size()));std::vector<int> e(a.begin()+lo,a.begin()+hi);CHECK((a|slice(l,r))==e);CHECK((std::vector<int>(a)|slice(l,r))==e);}
    std::vector<bool> booleans{true,false,true};CHECK((booleans|map([](bool x){return x;}))==booleans);CHECK((booleans|sorted())==std::vector<bool>({false,true,true}));CHECK((booleans|filter([](bool x){return x;}))==std::vector<bool>({true,true}));
    auto enumerated=a|enumerate();for(std::size_t i=0;i<a.size();++i)CHECK(enumerated[i]==std::pair<int,std::size_t>(a[i],i));CHECK((a|count([](int x){return x==1;}))==2);CHECK((a|first([](int x){return x==2;}))==2);CHECK(!(a|first([](int){return false;})));
    std::vector<std::unique_ptr<int>> owned;owned.push_back(std::make_unique<int>(7));owned.push_back(std::make_unique<int>(9));auto firstOwned=std::move(owned)|first([](const auto& x){return *x==9;});CHECK(firstOwned && **firstOwned==9);
    std::vector<Copies> records;records.emplace_back(1);records.emplace_back(2);Copies::count=0;CHECK((records|count([](const Copies& x){return x.value>0;}))==2);CHECK(Copies::count==0);
    auto input=std::istringstream("1 0 1");CHECK((std::vector<bool>(3)|seq::cin(input))==booleans);auto ints=std::istringstream("4 5 bad");CHECK((std::vector<int>{0,0,9,7}|seq::cin(ints))==std::vector<int>({4,5,0,7}));
    std::ostringstream output;booleans|seq::cout(output,"|",".");CHECK(output.str()=="1|0|1.");std::ostringstream stringOutput;std::string(view)|seq::cout(stringOutput,"-","");CHECK(stringOutput.str()==text);
    Box box{3};auto member=seq::memberCall(&Box::value);decltype(auto) borrowed=box|member;static_assert(std::is_same_v<decltype(borrowed),const int&>);CHECK(&borrowed==&box.value);
    auto modified=Box{1}|seq::memberCall(&Box::set,9);CHECK(modified.value==9);CHECK((box|call(get))==3);
    decltype(auto) fromTemporary=std::vector<std::string>{std::string(1000,'x')}|call(front);static_assert(std::is_same_v<decltype(fromTemporary),std::string>);CHECK(fromTemporary.size()==1000);
    auto getFront=call(front);decltype(auto) front=a|getFront;static_assert(std::is_same_v<decltype(front),const int&>);CHECK(&front==&a.front());CHECK((std::string("abc")|call(append,"def"))=="abcdef");
    auto suffix=call(substr,1,2);CHECK((std::string("abcd")|suffix)=="bc");CHECK((std::string("xyzw")|suffix)=="yz");
    std::cout<<"ListHelper: sort/radix oracles, ownership/views, reusable move-only ops, proxies, streams, member result lifetime PASS\n";
    return 0;
}

#include "../../../../src/Clarketech/ListHelper/code.hpp"
#include "../../../Support/CaseSupport.hpp"
#include <sstream>

namespace boundary_cases {

int run() {
    runCase("ListHelper/empty", [] {
        std::vector<int> a;
        CHECK((a | sorted() | unique()).empty());
        CHECK(!(a | first([](int) {
                    return true;
                })));
    });
    runCase("ListHelper/sort-borrows", [] {
        std::vector<int> a{4, 1, 4, 2};
        auto b = a;
        CHECK((a | sorted()) == std::vector<int>({1, 2, 4, 4}));
        CHECK(a == b);
    });
    runCase("ListHelper/wide-signed", [] {
        std::vector<long long> a{LLONG_MAX, 0, LLONG_MIN, -1};
        auto e = a;
        std::sort(e.begin(), e.end());
        CHECK((a | sorted()) == e);
    });
    runCase("ListHelper/descending", [] {
        CHECK((std::vector<int>{2, 1, 3} | sorted(std::greater<int>{})) ==
              std::vector<int>({3, 2, 1}));
    });
    runCase("ListHelper/unique-adjacent", [] {
        CHECK((std::vector<int>{1, 1, 2, 1, 1} | unique()) == std::vector<int>({1, 2, 1}));
    });
    runCase("ListHelper/filter-map", [] {
        CHECK((std::vector<int>{-2, -1, 0, 1, 2} | filter([](int x) {
                   return x % 2 == 0;
               }) |
               map([](int x) {
                   return x * x;
               })) == std::vector<int>({4, 0, 4}));
    });
    runCase("ListHelper/slice-clamp", [] {
        CHECK((std::vector<int>{1, 2, 3} | slice(1, 99)) == std::vector<int>({2, 3}));
        CHECK((std::vector<int>{1} | slice(4, 0)).empty());
    });
    runCase("ListHelper/reusable-state", [] {
        auto op = map([k = 0](int) mutable {
            return ++k;
        });
        std::vector<int> a{0, 0};
        CHECK((a | op) == std::vector<int>({1, 2}));
        CHECK((a | op) == std::vector<int>({3, 4}));
    });
    runCase("ListHelper/temporary-member", [] {
        auto s = std::vector<std::string>{std::string(4096, 'x')} | call(front);
        CHECK(s == std::string(4096, 'x'));
    });
    runCase("ListHelper/proxy-stream", [] {
        std::vector<bool> a{true, false, true};
        CHECK((a | map([](bool x) {
                   return !x;
               })) == std::vector<bool>({false, true, false}));
        std::ostringstream out;
        a | seq::cout(out, "|", ".");
        CHECK(out.str() == "1|0|1.");
    });
    return 0;
}
}

int main() {
    runCase("ListHelper/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
