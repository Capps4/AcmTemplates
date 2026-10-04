#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

using Clock=std::chrono::steady_clock;

template<class T,bool Descending>
__attribute__((noinline, aligned(64)))
void count(const std::vector<T>& a,std::array<std::size_t,256>& cnt,
           int shift,unsigned flip,T minV,T maxV) {
    using U=std::make_unsigned_t<T>;
    cnt.fill(0);
    for(std::size_t j=0;j<a.size();++j) {
        U key=Descending ? U(U(maxV)-U(a[j])) : U(U(a[j])-U(minV));
        ++cnt[((key>>shift)&255)^flip];
    }
}

template<class T,bool Descending>
void measure(const char* type,const char* direction) {
    std::mt19937_64 random(443971+sizeof(T));
    std::vector<T> input(1000000);
    for(auto& x:input) x=T((random()%256)<<16);
    std::array<std::size_t,256> cnt{};
    std::array<unsigned,4> flips{0,1,128,255};
    for(int shift:{0,8,16}) {
        for(int round=-1;round<11;++round) {
            std::shuffle(flips.begin(),flips.end(),random);
            for(unsigned flip:flips) {
                std::atomic_signal_fence(std::memory_order_seq_cst);
                auto start=Clock::now();
                count<T,Descending>(input,cnt,shift,flip,T(0),T(255<<16));
                auto stop=Clock::now();
                std::atomic_signal_fence(std::memory_order_seq_cst);
                std::size_t total=0;
                for(auto x:cnt) total+=x;
                if(total!=input.size() || (shift<16 && cnt[flip]!=input.size()))
                    throw std::runtime_error("histogram mismatch");
                if(round>=0)
                    std::cout<<type<<','<<direction<<','<<shift<<','<<flip<<','<<round<<','
                             <<std::chrono::duration<double,std::nano>(stop-start).count()<<'\n';
            }
        }
    }
}
int main() {
    std::cout<<"type,direction,shift,flip,round,ns\n"<<std::fixed<<std::setprecision(3);
    measure<int,false>("int","asc");
    measure<int,true>("int","desc");
    measure<long long,false>("longLong","asc");
    measure<long long,true>("longLong","desc");
}
