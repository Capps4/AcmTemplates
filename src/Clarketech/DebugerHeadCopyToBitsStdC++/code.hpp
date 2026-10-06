#pragma once
#include "../../../Headers/Headers.hpp"

#ifndef CAPPS_DEBUGER_HEAD
#ifndef COMPETITION_DEBUGER
#define CAPPS_DEBUGER_HEAD
#define COMPETITION_DEBUGER

namespace debuger {

struct Cfg {
    bool on = false;
    bool col = false;
    bool gap = false;
    int prec = 6;
};

// Function-local state is shared across translation units and safe at startup.
inline Cfg &cfg() {
    static Cfg a;
    return a;
}

inline bool &busy() {
    static thread_local bool a = false;
    return a;
}

struct Hold {
    Hold() {
        busy() = true;
    }
    ~Hold() {
        busy() = false;
    }
    Hold(const Hold &) = delete;
    Hold &operator=(const Hold &) = delete;
};

inline bool ws(char c) {
    return c == ' ' or c == '\t' or c == '\n' or c == '\r' or c == '\v' or c == '\f';
}

inline std::string_view trim(std::string_view s) {
    while (not s.empty() and ws(s.front())) {
        s.remove_prefix(1);
    }
    while (not s.empty() and ws(s.back())) {
        s.remove_suffix(1);
    }
    return s;
}

inline bool same(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        char c = a[i];
        if (c >= 'A' and c <= 'Z') {
            c = char(c - 'A' + 'a');
        }
        if (c != b[i]) {
            return false;
        }
    }
    return true;
}

inline char *init(std::string_view s) {
    Cfg a;
    while (not s.empty()) {
        size_t pos = s.find(',');
        auto opt = trim(s.substr(0, pos));
        s = pos == s.npos ? std::string_view{} : s.substr(pos + 1);
        size_t sep = opt.find(':');
        if (sep == opt.npos) {
            throw std::invalid_argument("debuger: expected key: value");
        }
        auto key = trim(opt.substr(0, sep));
        auto val = trim(opt.substr(sep + 1));
        if (same(key, "color") or same(key, "space")) {
            if (not same(val, "true") and not same(val, "false")) {
                throw std::invalid_argument("debuger: expected true or false");
            }
            (same(key, "color") ? a.col : a.gap) = same(val, "true");
        } else if (same(key, "precision")) {
            int n = 0;
            if (val.empty()) {
                throw std::invalid_argument("debuger: empty precision");
            }
            for (char c : val) {
                if (c < '0' or c > '9' or n > 30) {
                    throw std::invalid_argument("debuger: precision must be 0..30");
                }
                n = n * 10 + c - '0';
            }
            if (n > 30) {
                throw std::invalid_argument("debuger: precision must be 0..30");
            }
            a.prec = n;
        } else {
            throw std::invalid_argument("debuger: unknown option");
        }
    }
    auto p = static_cast<char *>(std::malloc(1));
    if (p == nullptr) {
        throw std::bad_alloc();
    }
    p[0] = '\0';
    a.on = true;
    cfg() = a;
    return p;
}

inline void esc(std::ostream &out, std::string_view s, char quo) {
    constexpr char hex[] = "0123456789abcdef";
    out.put(quo);
    for (unsigned char c : s) {
        if (c == '\\' or c == quo) {
            out.put('\\');
            out.put(char(c));
        } else if (c == '\n') {
            out << "\\n";
        } else if (c == '\r') {
            out << "\\r";
        } else if (c == '\t') {
            out << "\\t";
        } else if (c < 32 or c == 127) {
            out << "\\x" << hex[c / 16] << hex[c % 16];
        } else {
            out.put(char(c));
        }
    }
    out.put(quo);
}

// These traits select a representation, not a copy or an ownership policy.
template <class T, class = void>
struct Out : std::false_type {};
template <class T>
struct Out<T, std::void_t<decltype(std::declval<std::ostream &>() <<
                                 std::declval<const T &>())>> : std::true_type {};

template <class T, class = void>
struct Seq : std::false_type {};
template <class T>
struct Seq<T, std::void_t<decltype(std::begin(std::declval<const T &>())),
                          decltype(std::end(std::declval<const T &>()))>> : std::true_type {};

template <class T, class = void>
struct Key : std::false_type {};
template <class T>
struct Key<T, std::void_t<typename T::key_type>> : std::true_type {};

template <class T, class = void>
struct Map : std::false_type {};
template <class T>
struct Map<T, std::void_t<typename T::key_type, typename T::mapped_type>> : std::true_type {};

template <class T, class = void>
struct Tup : std::false_type {};
template <class T>
struct Tup<T, std::void_t<decltype(std::tuple_size<T>::value)>> : std::true_type {};

template <class T>
struct Text : std::false_type {};
template <class A, class B>
struct Text<std::basic_string<char, A, B>> : std::true_type {};
template <class A>
struct Text<std::basic_string_view<char, A>> : std::true_type {};

template <class T>
struct Opt : std::false_type {};
template <class T>
struct Opt<std::optional<T>> : std::true_type {};

template <class T>
struct Var : std::false_type {};
template <class... T>
struct Var<std::variant<T...>> : std::true_type {};

// C++17 has no general reflection. Only simple aggregates of <=8 fields are supported.
struct Any {
    template <class T>
    operator T() const;
};

template <class T, size_t... I>
auto fit(std::index_sequence<I...>) ->
    decltype(T{(static_cast<void>(I), Any{})...}, std::true_type{});
template <class T>
auto fit(...) -> std::false_type;

template <class T, size_t N = 0>
constexpr size_t cnt() {
    if constexpr (N == 9) {
        return N;
    } else if constexpr (decltype(fit<T>(std::make_index_sequence<N + 1>{}))::value) {
        return cnt<T, N + 1>();
    } else {
        return N;
    }
}

template <class T>
void put(std::ostream &out, const T &a);

template <class... T>
void pack(std::ostream &out, const T &...a) {
    size_t n = 0;
    [[maybe_unused]] auto one = [&](const auto &x) {
        if (n++ != 0) {
            out << ", ";
        }
        put(out, x);
    };
    (one(a), ...);
}

template <class T>
void agg(std::ostream &out, const T &a) {
    constexpr size_t n = cnt<T>();
    out << '{';
    if constexpr (n == 1) {
        const auto &[b] = a;
        pack(out, b);
    } else if constexpr (n == 2) {
        const auto &[b, c] = a;
        pack(out, b, c);
    } else if constexpr (n == 3) {
        const auto &[b, c, d] = a;
        pack(out, b, c, d);
    } else if constexpr (n == 4) {
        const auto &[b, c, d, e] = a;
        pack(out, b, c, d, e);
    } else if constexpr (n == 5) {
        const auto &[b, c, d, e, f] = a;
        pack(out, b, c, d, e, f);
    } else if constexpr (n == 6) {
        const auto &[b, c, d, e, f, g] = a;
        pack(out, b, c, d, e, f, g);
    } else if constexpr (n == 7) {
        const auto &[b, c, d, e, f, g, h] = a;
        pack(out, b, c, d, e, f, g, h);
    } else if constexpr (n == 8) {
        const auto &[b, c, d, e, f, g, h, i] = a;
        pack(out, b, c, d, e, f, g, h, i);
    }
    out << '}';
}

template <class T>
void put(std::ostream &out, const T &a) {
    using U = std::remove_cv_t<T>;
    if constexpr (std::is_same_v<U, char>) {
        esc(out, std::string_view(&a, 1), '\'');
    } else if constexpr (Text<U>::value) {
        esc(out, std::string_view(a.data(), a.size()), '"');
    } else if constexpr (std::is_array_v<U> and
                         std::is_same_v<std::remove_extent_t<U>, char>) {
        size_t n = 0;
        while (n < std::extent_v<U> and a[n] != '\0') {
            ++n;
        }
        esc(out, std::string_view(a, n), '"');
    } else if constexpr (std::is_same_v<U, char *> or std::is_same_v<U, const char *>) {
        if (a == nullptr) {
            out << "nullptr";
        } else {
            esc(out, std::string_view(a), '"');
        }
    } else if constexpr (std::is_null_pointer_v<U>) {
        out << "nullptr";
    } else if constexpr (std::is_pointer_v<U>) {
        if (a == nullptr) {
            out << "nullptr";
        } else if constexpr (std::is_function_v<std::remove_pointer_t<U>>) {
            out << "0x" << std::hex << reinterpret_cast<std::uintptr_t>(a) << std::dec;
        } else {
            out << const_cast<const void *>(static_cast<const volatile void *>(a));
        }
    } else if constexpr (Opt<U>::value) {
        if (a) {
            out << "optional(";
            put(out, *a);
            out << ')';
        } else {
            out << "nullopt";
        }
    } else if constexpr (Var<U>::value) {
        if (a.valueless_by_exception()) {
            out << "valueless";
        } else {
            out << "variant(" << a.index() << ": ";
            std::visit([&](const auto &x) {
                put(out, x);
            }, a);
            out << ')';
        }
    } else if constexpr (Out<U>::value and not std::is_array_v<U>) {
        out << a;
    } else if constexpr (Seq<U>::value) {
        out << (Key<U>::value ? '{' : '[');
        size_t n = 0;
        for (const auto &x : a) {
            if (n++ != 0) {
                out << ", ";
            }
            if constexpr (Map<U>::value) {
                const auto &[key, value] = x;
                out << '(';
                pack(out, key, value);
                out << ')';
            } else {
                put(out, x);
            }
        }
        out << (Key<U>::value ? '}' : ']');
    } else if constexpr (Tup<U>::value) {
        out << '(';
        std::apply([&](const auto &...x) {
            pack(out, x...);
        }, a);
        out << ')';
    } else if constexpr (std::is_aggregate_v<U> and not std::is_union_v<U> and
                         decltype(fit<U>(std::index_sequence<>{}))::value and cnt<U>() <= 8) {
        agg(out, a);
    } else {
        out << "<unprintable>";
    }
}

inline unsigned hash(std::string_view s) {
    unsigned h = 2166136261u;
    for (unsigned char c : s) {
        h = (h ^ c) * 16777619u;
    }
    return h;
}

template <class... T>
void log(int line, const char *expr, const T &...a) {
    if (busy()) {
        return;
    }
    if (not cfg().on) {
        throw std::logic_error("debuger: configure strdup before debug");
    }
    Hold hold;
    const Cfg opt = cfg();
    std::ostringstream out;
    out << std::boolalpha << std::fixed << std::setprecision(opt.prec);
    unsigned h = opt.col or opt.gap ? hash(expr) : 0;
    if (opt.col) {
        constexpr const char *col[] = {"\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};
        out << col[h % 5];
    }
    if (opt.gap) {
        out << std::string(h % 4 * 4, ' ');
    }
    out << std::setw(4) << line << ":";
    if constexpr (sizeof...(T) != 0) {
        out << ' ' << expr << " = ";
        pack(out, a...);
    }
    if (opt.col) {
        out << "\033[0m";
    }
    out << '\n';
    const auto s = out.str();
    std::cerr.write(s.data(), std::streamsize(s.size()));
}

} // namespace debuger

// GNU C++17's comma elision also permits debug() with no arguments.
#define debug(...) ::debuger::log(__LINE__, #__VA_ARGS__, ##__VA_ARGS__)
// Keep the original one-line startup. (strdup)(s) bypasses this macro when needed.
#define strdup(...) ::debuger::init(__VA_ARGS__)

#endif
#endif
