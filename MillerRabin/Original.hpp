#pragma once
#include <bits/stdc++.h>
using i64 = long long;
/*
维基百科 :
n < 4e9, primes = [2, 7, 61]
n < 3e14, primes = [2, 3, 5, 7, 11, 13, 17]
n < 3e18, primes = [2, 3, 5, 7, 11, 13, 17, 19, 23]
n < 3e23, primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37]
*/
template <class T>
class MillerRabin {
    using R = ModuloInteger<T, 0>;
    const std::vector<int> primes;

    std::vector<int> getPrimes(int) {
        return std::vector<int>{2, 7, 61};
    }
    std::vector<int> getPrimes(long long) {
        return std::vector<int>{2, 3, 5, 7, 11, 13, 17, 19, 23};
    }

public:
    MillerRabin() : primes(getPrimes(T{})) {}

    constexpr bool operator()(T v) { // 判断v是不是质数
        if (v < 2 or (v != 2 and v % 2 == 0) or (v != 3 and v % 3 == 0)) {
            return false;
        }
        R::setMod(v);
        T s = v - 1;
        while ((s & 1) == 0) {
            s /= 2;
        }
        for (int p : primes) {
            if (v == p) {
                return true;
            }
            T t = s;
            R m = R(p).power(s);
            while (t + 1 != v and m - 1 != 0 and m + 1 != 0) {
                m = m * m;
                t *= 2;
            }

            if (m + 1 != 0 and (t & 1) == 0) {
                return false;
            }
        }
        return true;
    }
};
MillerRabin<long long> isPrime;

