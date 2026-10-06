#pragma once
#include "../../../../Headers/Headers.hpp"

// SNIPPET BEGIN
namespace _segt {
template <class Tag>
struct Tags {
    std::vector<Tag> tag{};
};

template <>
struct Tags<void> {};
}

template <class Info, class Tag = void>
class SegTree : private _segt::Tags<Tag> {
    static constexpr bool lazy = !std::is_void_v<Tag>;
    static constexpr int DMax = std::numeric_limits<int>::digits;
    int n = 0, base = 1, dep = 0;
    std::vector<Info> info{};

    // h is the distance to the leaf layer; ignore padding on the right.
    void up(int p, int h) {
        if (((p * 2 + 1) << (h - 1)) < base + n)
            info[p] = info[p * 2] + info[p * 2 + 1];
        else
            info[p] = info[p * 2];
    }
    template <class U>
    void apply(int p, const U &v) {
        info[p].apply(v);
        if (p < base)
            this->tag[p].apply(v);
    }

    void down(int p) {
        if constexpr (lazy) {
            apply(p * 2, this->tag[p]);
            apply(p * 2 + 1, this->tag[p]);
            this->tag[p] = Tag();
        }
    }
    void push(int p) {
        if constexpr (lazy)
            for (int h = dep; h; --h)
                down(p >> h);
    }

    void push(int l, int r) {
        if constexpr (lazy) {
            for (int h = dep; h; --h) {
                if (((l >> h) << h) != l)
                    down(l >> h);
                if (((r >> h) << h) != r)
                    down((r - 1) >> h);
            }
        }
    }

    template <class F>
    std::optional<int> find(int p, F &pred, bool last) {
        std::array<int, DMax> stk{};
        int cnt = 0;
        while (true) {
            if (pred(info[p])) {
                if (p >= base)
                    return p - base;
                down(p);
                p = p * 2 + int(last);
                stk[cnt++] = p ^ 1;
            } else {
                if (!cnt)
                    return std::nullopt;
                p = stk[--cnt];
            }
        }
    }

public:
    template <class G>
    explicit SegTree(const std::vector<G> &a) {
        n = static_cast<int>(a.size());
        if (!n)
            return;
        while (base < n) {
            base *= 2, ++dep;
        }
        info.assign(static_cast<std::size_t>(base) * 2, Info(a[0]));
        if constexpr (lazy)
            this->tag.assign(base, Tag());
        for (int i = 0; i < n; ++i)
            info[base + i] = Info(a[i]);
        for (int h = 1; h <= dep; ++h)
            for (int p = base >> h; p <= ((base + n - 1) >> h); ++p)
                up(p, h);
    }

    SegTree(int n, const Info &leaf) : SegTree(std::vector<Info>(n, leaf)) {}

    void modify(int p, const Info &v) {
        assert(0 <= p and p < n);
        p += base;
        push(p);
        info[p] = v;
        for (int h = 1; p > 1; ++h) {
            p >>= 1;
            up(p, h);
        }
    }
    // All intervals are [l, r).
    Info query(int l, int r) {
        assert(0 <= l and l < r and r <= n);
        l += base, r += base;
        push(l, r);
        std::optional<Info> a, b;
        while (l < r) {
            if (l & 1) {
                a = a ? *a + info[l] : info[l];
                ++l;
            }
            if (r & 1) {
                --r;
                b = b ? info[r] + *b : info[r];
            }
            l >>= 1, r >>= 1;
        }
        if (!a)
            return *b;
        if (!b)
            return *a;
        return *a + *b;
    }
    // U must equal Tag, so deduction cannot enable updates for Tag=void.
    template <class U = Tag,
              std::enable_if_t<!std::is_void_v<U> and std::is_same_v<U, Tag>, int> = 0>
    void modify(int l, int r, const U &v) {
        assert(0 <= l and l <= r and r <= n);
        if (l == r)
            return;
        l += base, r += base;
        push(l, r);
        int a = l, b = r;
        while (l < r) {
            if (l & 1)
                apply(l++, v);
            if (r & 1)
                apply(--r, v);
            l >>= 1, r >>= 1;
        }
        // Fully covered nodes retain their newly applied tags.
        for (int h = 1; h <= dep; ++h) {
            if (((a >> h) << h) != a)
                up(a >> h, h);
            if (((b >> h) << h) != b)
                up((b - 1) >> h, h);
        }
    }
    // A false predicate must rule out every matching leaf in that node.
    template <class F>
    std::optional<int> first(int l, int r, F pred) {
        assert(0 <= l and l <= r and r <= n);
        if (l == r)
            return std::nullopt;
        l += base, r += base;
        push(l, r);
        std::array<int, DMax> stk{};
        int cnt = 0;
        while (l < r) {
            if (l & 1) {
                auto ans = find(l++, pred, false);
                if (ans)
                    return ans;
            }
            if (r & 1)
                stk[cnt++] = --r;
            l >>= 1, r >>= 1;
        }
        while (cnt) {
            auto ans = find(stk[--cnt], pred, false);
            if (ans)
                return ans;
        }
        return std::nullopt;
    }

    template <class F>
    std::optional<int> last(int l, int r, F pred) {
        assert(0 <= l and l <= r and r <= n);
        if (l == r)
            return std::nullopt;
        l += base, r += base;
        push(l, r);
        std::array<int, DMax> stk{};
        int cnt = 0;
        while (l < r) {
            if (l & 1)
                stk[cnt++] = l++;
            if (r & 1) {
                auto ans = find(--r, pred, true);
                if (ans)
                    return ans;
            }
            l >>= 1, r >>= 1;
        }
        while (cnt) {
            auto ans = find(stk[--cnt], pred, true);
            if (ans)
                return ans;
        }
        return std::nullopt;
    }
};

// replace me: range add and range sum.
struct Tag {
    long long add = 0;

    void apply(const Tag &v) { add += v.add; }
};

struct Info {
    long long val;
    int len;

    explicit Info(long long val = 0, int len = 1) : val(val), len(len) {}

    Info operator+(const Info &v) const { return Info(val + v.val, len + v.len); }

    void apply(const Tag &v) { val += v.add * len; }
};
