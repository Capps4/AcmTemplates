#pragma once
#include <algorithm>
#include <cassert>
#include <limits>
#include <string>
#include <utility>

struct AddTag {
    long long add = 0;
    void apply(const AddTag &v) { add += v.add; }
};

struct AffineTag {
    long long mul = 1, add = 0;
    void apply(const AffineTag &v) { mul *= v.mul; add = add * v.mul + v.add; }
};

struct SumInfo {
    long long val;
    int len;
    explicit SumInfo(long long val, int len = 1) : val(val), len(len) { assert(len > 0); }
    SumInfo operator+(const SumInfo &v) const { return SumInfo(val + v.val, len + v.len); }
    void apply(const AddTag &v) { val += v.add * len; }
    void apply(const AffineTag &v) { val = val * v.mul + v.add * len; }
};

struct MaxInfo {
    long long val;
    int len;
    explicit MaxInfo(long long val, int len = 1) : val(val), len(len) {}
    MaxInfo operator+(const MaxInfo &v) const { return MaxInfo(std::max(val, v.val), len + v.len); }
    void apply(const AddTag &v) { if (len) val += v.add; }
};

struct TextInfo {
    std::string val;
    explicit TextInfo(std::string val) : val(std::move(val)) { assert(!this->val.empty()); }
    explicit TextInfo(char c) : val(1, c) {}
    TextInfo operator+(const TextInfo &v) const { return TextInfo(val + v.val); }
};

struct SetTag {
    char c = 0;
    bool set = false;
    void apply(const SetTag &v) { if (v.set) { c = v.c; set = true; } }
};

struct SetInfo : TextInfo {
    using TextInfo::TextInfo;
    SetInfo operator+(const SetInfo &v) const { return SetInfo(val + v.val); }
    void apply(const SetTag &v) { if (v.set) std::fill(val.begin(), val.end(), v.c); }
};
