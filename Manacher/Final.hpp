#pragma once
#include <algorithm>
#include <cassert>
#include <string_view>
#include <vector>

// SNIPPET BEGIN
class Manacher {
    const int n;
    std::vector<int> odd, even, tail;

public:
    explicit Manacher(std::string_view s) : n(s.size()), odd(n), even(n), tail(n) {
        for (int i = 0, l = 0, r = -1; i < n; ++i) {
            int rad = i > r ? 1 : std::min(odd[l + (r - i)], r - i + 1);
            while (rad <= i && rad < n - i && s[i - rad] == s[i + rad])
                ++rad;
            odd[i] = rad;
            if (i + rad - 1 > r) {
                l = i - rad + 1;
                r = i + rad - 1;
            }
            tail[i + rad - 1] = std::max(tail[i + rad - 1], rad + (rad - 1));
        }
        // even[i] is centered between i-1 and i.
        for (int i = 0, l = 0, r = -1; i < n; ++i) {
            int rad = i > r ? 0 : std::min(even[l + (r - i) + 1], r - i + 1);
            while (rad < i && rad < n - i && s[i - rad - 1] == s[i + rad])
                ++rad;
            even[i] = rad;
            if (i + rad - 1 > r) {
                l = i - rad;
                r = i + rad - 1;
            }
            if (rad) tail[i + rad - 1] = std::max(tail[i + rad - 1], 2 * rad);
        }
        for (int i = n - 2; i >= 0; --i)
            tail[i] = std::max(tail[i], tail[i + 1] - 2);
    }

    // Returns the full len; mid=true means the center i+0.5.
    int getPalinLenFromCenter(int i, bool mid = false) const {
        assert(i >= 0 && i < n - int(mid));
        return mid ? 2 * even[i + 1] : odd[i] + (odd[i] - 1);
    }

    int getPalinLenFromTail(int i) const {
        assert(i >= 0 && i < n);
        return tail[i];
    }

    bool isPalindrome(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return true;
        int len = r - l;
        return getPalinLenFromCenter(l + (len - 1) / 2, len % 2 == 0) >= len;
    }
};
