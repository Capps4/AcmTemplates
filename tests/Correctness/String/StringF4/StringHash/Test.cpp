#include "../../../../../src/String/StringF4/StringHash/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <string>
inline constexpr int SingleBase[]{3},SingleMod[]{101};
inline constexpr int TripleBase[]{3,5,7},TripleMod[]{101,103,107};
_strhash::u64 otherHash(std::string_view text);
const int* otherHashBases();
template<class Seq,int D,const int* B,const int* P>
std::array<int,D> reference(const Seq& sequence,int l,int r){
    std::array<int,D> result{};
    using I=std::decay_t<decltype(sequence[0])>;
    for(int i=l;i<r;++i)for(int k=0;k<D;++k){
        unsigned long long symbol;
        if constexpr(std::is_same_v<I,char> || std::is_same_v<I,unsigned char> || std::is_same_v<I,signed char>) symbol=static_cast<unsigned char>(sequence[i])+1;
        else if constexpr(std::is_signed_v<I>){__int128 value=sequence[i];value=(value%P[k]+P[k]+1)%P[k];symbol=static_cast<unsigned long long>(value);}
        else{auto value=static_cast<unsigned __int128>(sequence[i]);symbol=static_cast<unsigned long long>((value%P[k]+1)%P[k]);}
        result[k]=int((static_cast<unsigned __int128>(result[k])*B[k]+symbol)%P[k]);
    }
    return result;
}
template<class Seq,int D,const int* B,const int* P>
void verify(const Seq& sequence){
    _strhash::Impl<D,B,P> hash(sequence);int n=int(sequence.size());
    CHECK(hash.getArray(n,n)==std::array<int,D>{}); // First query must support the empty interval.
    for(int l=0;l<=n;++l)for(int r=l;r<=n;++r){CHECK(hash.getArray(l,r)==reference<Seq,D,B,P>(sequence,l,r));if constexpr(D<=2){auto arr=hash.getArray(l,r);_strhash::u64 packed=arr[0];if constexpr(D==2)packed|=_strhash::u64(arr[1])<<32;CHECK(hash.getU64(l,r)==packed);}}
    if constexpr(D<=2)CHECK(hash.getU64(0)==hash.getU64(0,n));
}
int main(){
    for (int k=0;k<_strhash::d;++k) CHECK(_strhash::p[k]/2 < _strhash::b[k] && _strhash::b[k] < _strhash::p[k]);
    StringHash empty("");CHECK(empty.getU64(0)==0);
    verify<std::string,2,_strhash::b,_strhash::p>("abcdef");
    for(int test=0;test<500;++test){std::string text(randomInt(0,70),'\0');for(auto& c:text)c=char(randomInt(0,255));verify<std::string,2,_strhash::b,_strhash::p>(text);verify<std::string_view,1,SingleBase,SingleMod>(text);}
    std::vector<long long> signedValues{std::numeric_limits<long long>::min(),std::numeric_limits<long long>::max(),-1,0,1,-1000000021LL};
    verify<std::vector<long long>,2,_strhash::b,_strhash::p>(signedValues);
    std::vector<unsigned long long> unsignedValues{0,1,std::numeric_limits<unsigned long long>::max(),1ULL<<63};verify<std::vector<unsigned long long>,2,_strhash::b,_strhash::p>(unsignedValues);
    verify<std::vector<bool>,2,_strhash::b,_strhash::p>(std::vector<bool>{true,false,true});
    verify<std::string,3,TripleBase,TripleMod>("triple");
    std::string text="abacabac";StringHash hash(text);CHECK(hash.getArray(0,4)==hash.getArray(4,8));
    StringHash owned(std::string("abcdef"));CHECK(owned.getArray(1,4)==StringHash("bcd").getArray(0,3));
    CHECK(otherHashBases()==_strhash::b);CHECK(otherHash("abcabc")==StringHash("abcabc").getU64(0));
    // Growing the shared cache must preserve hashes of already constructed objects.
    std::string longer(2000, 'z');StringHash grown(longer);
    CHECK(grown.getArray(0,2000)==reference<std::string,2,_strhash::b,_strhash::p>(longer,0,2000));
    CHECK(hash.getArray(0,4)==hash.getArray(4,8));
    StringHash copy=hash;CHECK(copy.getU64(0)==hash.getU64(0));
    StringHash shorter("x");CHECK(shorter.getU64(0)==StringHash("xx").getU64(1));
    std::cout<<"StringHash: independent polynomial oracle, empty intervals first, bytes, numeric extremes, dimensions, multi-TU PASS; bases="<<_strhash::b[0]<<','<<_strhash::b[1]<<'\n';
}
