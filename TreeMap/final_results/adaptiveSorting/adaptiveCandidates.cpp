#include "legacy/sort.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
using Clock = std::chrono::steady_clock;
template<class T>
void currentSort(std::vector<T>& keys) {
    using Key = T;
    using Compare = std::less<T>;
    Compare compare;
        if (keys.size() < 1024) {
            std::sort(keys.begin(), keys.end(), compare);
            return;
        }
        if constexpr (std::is_integral_v<Key> && !std::is_same_v<Key, bool> &&
                      (std::is_same_v<Compare, std::less<Key>> ||
                       std::is_same_v<Compare, std::greater<Key>>)) {
            using Unsigned = std::make_unsigned_t<Key>;
            constexpr int bits = std::numeric_limits<Unsigned>::digits;
            std::vector<Key> buffer(keys.size());
            for (int shift = 0; shift < bits; shift += 8) {
                auto bucket = [shift](Key key) {
                    Unsigned ordered = static_cast<Unsigned>(key);
                    if constexpr (std::is_signed_v<Key>)
                        ordered ^= Unsigned(1) << (bits - 1);
                    unsigned digit = (ordered >> shift) & 255;
                    if constexpr (std::is_same_v<Compare, std::greater<Key>>)
                        digit ^= 255;
                    return digit;
                };
                size_t positions[256]{};
                for (Key key : keys)
                    ++positions[bucket(key)];
                if (positions[bucket(keys.front())] == keys.size())
                    continue;
                size_t total = 0;
                for (size_t& position : positions) {
                    size_t count = position;
                    position = total;
                    total += count;
                }
                for (Key key : keys)
                    buffer[positions[bucket(key)]++] = key;
                keys.swap(buffer);
            }
        } else {
            std::sort(keys.begin(), keys.end(), compare);
        }
    }


template<class T> void adaptive(std::vector<T>& a){sorting::sort(a);}
template<class T> void standard(std::vector<T>& a){std::sort(a.begin(),a.end());}
template<class T>
void sweep(const char* type) {
    std::mt19937_64 rng(2026100107);
    using Run=void (*)(std::vector<T>&);
    std::array<Run,3> runs{standard<T>,currentSort<T>,adaptive<T>};
    const char* labels[]={"stdSort","current8","adaptive"};
    const char* distributions[]={"positive22","signed16","full","few","reverse"};
    for(int n:{16,64,256,512,1024,2048,4096,16384,65536,1000000,2000000}) {
        for(int kind=0;kind<5;++kind) {
            std::vector<T> input(n);
            for(int i=0;i<n;++i) {
                auto x=rng();
                if(kind==0) input[i]=static_cast<T>(x&((1u<<22)-1));
                if(kind==1) input[i]=static_cast<T>(int(x&65535)-32768);
                if(kind==2) input[i]=static_cast<T>(x);
                if(kind==3) input[i]=static_cast<T>(x&15);
                if(kind==4) input[i]=static_cast<T>(n-i);
            }
            auto expected=input; std::sort(expected.begin(),expected.end());
            auto bounds=std::minmax_element(input.begin(),input.end());
            using U=std::make_unsigned_t<T>;
            U maxV=static_cast<U>(*bounds.second)-static_cast<U>(*bounds.first);
            int bits=0; for(U v=maxV;v;v>>=1)++bits;
            std::array<int,3> order{0,1,2};
            int repeats=std::max(1,std::min(64,8192/n));
            for(int round=-1;round<5;++round) {
                std::shuffle(order.begin(),order.end(),rng);
                for(int c:order) {
                    double total=0;
                    for(int k=0;k<repeats;++k) {
                        auto a=input;
                        std::atomic_signal_fence(std::memory_order_seq_cst);
                        auto start=Clock::now(); runs[c](a); auto stop=Clock::now();
                        std::atomic_signal_fence(std::memory_order_seq_cst);
                        if(a!=expected) throw std::runtime_error("sort mismatch");
                        total+=std::chrono::duration<double,std::nano>(stop-start).count();
                    }
                    if(round>=0) std::cout<<type<<','<<n<<','<<distributions[kind]<<','<<bits<<','<<labels[c]<<','<<round+1<<','<<total/repeats<<'\n';
                }
            }
            std::cout.flush();
            std::cerr<<type<<' '<<n<<' '<<distributions[kind]<<" verified\n";
        }
    }
}
int main(){
    std::cout<<"type,n,distribution,bits,algorithm,round,ns\n"<<std::fixed<<std::setprecision(3);
    sweep<int>("int"); sweep<long long>("longLong");
}
