#include "legacy.hpp"
#include "current.hpp"
#include "swap.hpp"
#include "forward.hpp"
#include "forwardSwap.hpp"
#include "heapBuckets.hpp"
#include "invertedDigits.hpp"
#include "combined.hpp"
#include <atomic>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>

using Clock=std::chrono::steady_clock;

template<int Method, class T, class Compare>
__attribute__((noinline, aligned(64)))
void run(std::vector<T>& a, Compare cmp) {
    if constexpr(Method==0) legacy::sortKeys(a,cmp);
    if constexpr(Method==1) a=std::move(a)|current::sorted(cmp);
    if constexpr(Method==2) a=std::move(a)|swap::sorted(cmp);
    if constexpr(Method==3) a=std::move(a)|forward::sorted(cmp);
    if constexpr(Method==4) a=std::move(a)|forwardSwap::sorted(cmp);
    if constexpr(Method==5) a=std::move(a)|heapBuckets::sorted(cmp);
    if constexpr(Method==6) a=std::move(a)|invertedDigits::sorted(cmp);
    if constexpr(Method==7) a=std::move(a)|combined::sorted(cmp);
}

template<class T,class Compare>
void measure(const char* type,int kind,Compare cmp,const char* direction) {
    constexpr int n=1000000;
    std::mt19937_64 random(977193+kind*17+sizeof(T));
    std::vector<T> input(n);
    for(auto& x:input) {
        if(kind==0) x=T((random()%256)<<16);
        if(kind==1) x=T(random()%16)-8;
        if(kind==2) x=T(random()%(1<<22));
        if(kind==3) {
            using U=std::make_unsigned_t<T>;
            U bits=U(random());
            std::memcpy(&x,&bits,sizeof(T));
        }
    }
    if(kind==3) {
        input[0]=std::numeric_limits<T>::min();
        input[1]=std::numeric_limits<T>::max();
    }
    auto expected=input;
    std::sort(expected.begin(),expected.end(),cmp);
    using Fn=void (*)(std::vector<T>&,Compare);
    std::array<std::pair<int,Fn>,8> methods{{
        {0,run<0,T,Compare>},{1,run<1,T,Compare>},{2,run<2,T,Compare>},{3,run<3,T,Compare>},
        {4,run<4,T,Compare>},{5,run<5,T,Compare>},{6,run<6,T,Compare>},{7,run<7,T,Compare>}}};
    const char* names[]={"gaps","fewValues","range22","fullWidth"};
    for(int round=-1;round<9;++round) {
        std::shuffle(methods.begin(),methods.end(),random);
        for(auto [method,function]:methods) {
            auto a=input;
            std::atomic_signal_fence(std::memory_order_seq_cst);
            auto start=Clock::now();
            function(a,cmp);
            auto stop=Clock::now();
            std::atomic_signal_fence(std::memory_order_seq_cst);
            if(a!=expected) throw std::runtime_error("factorial sorting mismatch");
            if(round>=0)
                std::cout<<type<<','<<names[kind]<<','<<direction<<','<<method<<','<<round<<','
                         <<std::chrono::duration<double,std::nano>(stop-start).count()<<'\n';
        }
    }
    std::cerr<<type<<' '<<names[kind]<<' '<<direction<<" verified\n";
}

template<class T>
void sweep(const char* type) {
    for(int kind=0;kind<4;++kind) {
        measure<T>(type,kind,std::less<T>{},"asc");
        measure<T>(type,kind,std::greater<T>{},"desc");
    }
}
int main() {
    std::cout<<"type,distribution,direction,method,round,ns\n"<<std::fixed<<std::setprecision(3);
    sweep<int>("int");
    sweep<long long>("longLong");
}
