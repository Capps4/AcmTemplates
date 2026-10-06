#include "../../../../src/DataStructures/BaseDataStructures/HashMap/code.hpp"
#include "../../../../src/DataStructures/TreeDataStructures/TreeMap/code.hpp"
#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"
#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"
#include "../../../Support/CaseSupport.hpp"

debuger::Cfg *otherConfig();
bool *otherBusy();
void otherLog(double value);
std::string otherMaps();

namespace fixture {
// A mapping unrelated to library types exercises the public printing contract.
struct Mapping {
    using key_type = int;
    using mapped_type = std::vector<int>;
    struct Entry { int key; mapped_type value; };
    std::array<Entry, 1> entries{{{1, {2, 3}}}};
    auto begin() const { return entries.begin(); }
    auto end() const { return entries.end(); }
};

struct Capture {
    std::ostringstream out{};
    std::streambuf *old;
    Capture() : out(), old(std::cerr.rdbuf(out.rdbuf())) {}
    ~Capture() { std::cerr.rdbuf(old); }
    Capture(const Capture &) = delete;
    Capture &operator=(const Capture &) = delete;
};
template <class F>
std::string logged(F body) {
    Capture capture;
    body();
    return capture.out.str();
}
void configure(std::string_view options = "") {
    std::free(debuger::init(options));
}
template <class T>
std::string render(const T &value) {
    std::ostringstream out;
    out << std::boolalpha << std::fixed << std::setprecision(6);
    debuger::put(out, value);
    return out.str();
}
template <class Exception, class F>
void throws(F body) {
    bool caught = false;
    try {
        body();
    } catch (const Exception &) {
        caught = true;
    }
    CHECK(caught);
}

// Decode the output rather than reproducing the escaping implementation.
std::string decode(const std::string &text) {
    CHECK(text.size() >= 2 and text.front() == '"' and text.back() == '"');
    std::string result;
    for (std::size_t i = 1; i + 1 < text.size(); ++i) {
        char c = text[i];
        if (c != '\\') {
            result += c;
            continue;
        }
        CHECK(++i + 1 < text.size());
        c = text[i];
        if (c == 'n') result += '\n';
        else if (c == 'r') result += '\r';
        else if (c == 't') result += '\t';
        else if (c == 'x') {
            CHECK(i + 3 < text.size());
            result += char(std::stoi(text.substr(i + 1, 2), nullptr, 16));
            i += 2;
        } else {
            CHECK(c == '\\' or c == '"');
            result += c;
        }
    }
    return result;
}
std::map<u64, int> parseMap(const std::string &text) {
    std::istringstream in(text);
    char token = 0;
    in >> token;
    CHECK(token == '{');
    std::map<u64, int> result;
    if (in.peek() != '}') {
        do {
            u64 key = 0;
            int value = 0;
            in >> token;
            CHECK(token == '(');
            in >> key >> token;
            CHECK(in and token == ',');
            in >> value >> token;
            CHECK(in and token == ')');
            CHECK(result.emplace(key, value).second);
            in >> token;
            CHECK(in and (token == ',' or token == '}'));
        } while (token == ',');
    } else {
        in >> token;
    }
    CHECK(token == '}' and in.peek() == std::char_traits<char>::eof());
    return result;
}
template <class Map>
std::string plainMap(const Map &map) {
    std::ostringstream out;
    out << '{';
    bool first = true;
    for (const auto &[key, value] : map) {
        if (not first) out << ", ";
        first = false;
        out << '(' << key << ", " << value << ')';
    }
    out << '}';
    return out.str();
}

struct A0 {};
struct A1 { int a = 1; };
struct A2 { int a = 1, b = 2; };
struct A3 { int a = 1, b = 2, c = 3; };
struct A4 { int a = 1, b = 2, c = 3, d = 4; };
struct A5 { int a = 1, b = 2, c = 3, d = 4, e = 5; };
struct A6 { int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6; };
struct A7 { int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7; };
struct A8 { int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8; };
struct A9 { int a = 1, b = 2, c = 3, d = 4, e = 5, f = 6, g = 7, h = 8, i = 9; };
union Union { int value = 0; };
class Unknown {
    int value = 0;
public:
    Unknown() {}
};
struct Owned {
    int value;
    Owned() noexcept : value(0) {}
    explicit Owned(int v) noexcept : value(v) {}
    Owned(const Owned &) = delete;
    Owned &operator=(const Owned &) = delete;
    Owned(Owned &&) noexcept = default;
    Owned &operator=(Owned &&) noexcept = default;
};
std::ostream &operator<<(std::ostream &out, const Owned &value) {
    return out << "owned(" << value.value << ')';
}
struct WithOwned { Owned value{}; int flag = 2; };
struct Copies {
    inline static int count = 0;
    int value = 0;
    Copies() noexcept = default;
    Copies(const Copies &other) noexcept : value(other.value) { ++count; }
    Copies &operator=(const Copies &) noexcept = default;
    Copies(Copies &&) noexcept = default;
    Copies &operator=(Copies &&) noexcept = default;
};
std::ostream &operator<<(std::ostream &out, const Copies &value) {
    return out << value.value;
}
struct Priority {
    int values[2]{1, 2};
    const int *begin() const { return values; }
    const int *end() const { return values + 2; }
};
std::ostream &operator<<(std::ostream &out, const Priority &) {
    return out << "preferred";
}
struct Nested { int *count; };
std::ostream &operator<<(std::ostream &out, const Nested &value) {
    debug(++*value.count);
    return out << "outer";
}
struct Broken {};
std::ostream &operator<<(std::ostream &, const Broken &) {
    throw std::runtime_error("formatter failure");
}
struct VariantFailure {
    explicit VariantFailure(bool fail = false) {
        if (fail) throw std::runtime_error("variant failure");
    }
    VariantFailure(const VariantFailure &) = delete;
    VariantFailure &operator=(const VariantFailure &) = delete;
    VariantFailure(VariantFailure &&) noexcept(false) { throw std::runtime_error("move failure"); }
    VariantFailure &operator=(VariantFailure &&) noexcept(false) { throw std::runtime_error("move failure"); }
};
void functionPointer() {}
}

// ADL must continue to select a user formatter ahead of the map adapter.
namespace _hashmap {
std::ostream &operator<<(std::ostream &out, const Impl<int, 7, 8> &) {
    return out << "custom-map";
}
}

int main() {
    using namespace fixture;
    runCase("DebugerHeadCopyToBitsStdC++/configuration-required", [] {
        debuger::cfg() = debuger::Cfg{};
        throws<std::logic_error>([] { debuger::log(1, "x", 1); });
        CHECK(not debuger::busy());
    });
    runCase("DebugerHeadCopyToBitsStdC++/configuration-valid", [] {
        configure(" CoLoR : TRUE , SPACE : FaLsE , precision : 007 ");
        CHECK(debuger::cfg().on and debuger::cfg().col and not debuger::cfg().gap);
        CHECK(debuger::cfg().prec == 7);
        configure();
        CHECK(debuger::cfg().on and not debuger::cfg().col and not debuger::cfg().gap);
        CHECK(debuger::cfg().prec == 6);
        char *native = (strdup)("native copy");
        CHECK(native and std::strcmp(native, "native copy") == 0);
        std::free(native);
        char *options = strdup("precision: 0");
        CHECK(options and options[0] == '\0' and debuger::cfg().prec == 0);
        std::free(options);
    });
    runCase("DebugerHeadCopyToBitsStdC++/configuration-rollback", [] {
        configure("color: true, space: true, precision: 7");
        for (const char *bad : {"precision:-1", "precision:31", "precision:999999999999999",
                               "precision:", "color:yes", "space:0", "other:true", "color",
                               ":false", ",", "precision:2, space:wrong"}) {
            throws<std::invalid_argument>([&] { configure(bad); });
            CHECK(debuger::cfg().on and debuger::cfg().col and debuger::cfg().gap);
            CHECK(debuger::cfg().prec == 7);
        }
    });
    runCase("DebugerHeadCopyToBitsStdC++/scalars-and-precision", [] {
        configure();
        CHECK_EQ(render(true), "true");
        CHECK_EQ(render(false), "false");
        CHECK_EQ(render(std::numeric_limits<long long>::min()), "-9223372036854775808");
        CHECK_EQ(render(std::numeric_limits<unsigned long long>::max()), "18446744073709551615");
        CHECK_EQ(logged([] { debuger::log(12, "x", 1.5); }), "  12: x = 1.500000\n");
        configure("precision:0");
        CHECK_EQ(logged([] { debuger::log(12, "x", 1.5); }), "  12: x = 2\n");
        configure("precision:30");
        CHECK_EQ(logged([] { debuger::log(12, "x", 1.5); }),
                 std::string("  12: x = 1.5") + std::string(29, '0') + "\n");
    });
    runCase("DebugerHeadCopyToBitsStdC++/text-bytes-and-arrays", [] {
        CHECK_EQ(render(std::string("\"\\\n\r\t\0\x7f", 7)), "\"\\\"\\\\\\n\\r\\t\\x00\\x7f\"");
        CHECK_EQ(render('\''), "'\\''");
        CHECK_EQ(render('\0'), "'\\x00'");
        std::string bytes;
        for (int i = 0; i < 256; ++i) bytes += char(i);
        CHECK_EQ(decode(render(bytes)), bytes);
        CHECK_EQ(render(std::string_view(bytes.data() + 1, 3)), "\"\\x01\\x02\\x03\"");
        const char bounded[3]{'a', 'b', 'c'};
        const char terminated[4]{'a', '\0', 'x', 'y'};
        CHECK_EQ(render(bounded), "\"abc\"");
        CHECK_EQ(render(terminated), "\"a\"");
        CHECK_EQ(render(""), "\"\"");
    });
    runCase("DebugerHeadCopyToBitsStdC++/pointers", [] {
        CHECK_EQ(render(nullptr), "nullptr");
        CHECK_EQ(render(static_cast<const char *>(nullptr)), "nullptr");
        CHECK_EQ(render(static_cast<int *>(nullptr)), "nullptr");
        const char *text = "text";
        CHECK_EQ(render(text), "\"text\"");
        volatile int value = 3;
        std::ostringstream address;
        address << const_cast<const void *>(static_cast<const volatile void *>(&value));
        CHECK_EQ(render(&value), address.str());
        std::ostringstream function;
        function << "0x" << std::hex << reinterpret_cast<std::uintptr_t>(&functionPointer);
        CHECK_EQ(render(&functionPointer), function.str());
    });
    runCase("DebugerHeadCopyToBitsStdC++/standard-containers", [] {
        CHECK_EQ(render(std::vector<int>{}), "[]");
        CHECK_EQ(render(std::vector<bool>{true, false}), "[true, false]");
        CHECK_EQ(render(std::array<int, 2>{3, 4}), "[3, 4]");
        CHECK_EQ(render(std::set<int, std::greater<int>>{1, 3, 2}), "{3, 2, 1}");
        CHECK_EQ(render(std::multiset<int>{1, 1, 2}), "{1, 1, 2}");
        CHECK_EQ(render(std::map<int, std::string>{{1, "a"}, {2, "b"}}), "{(1, \"a\"), (2, \"b\")}");
        CHECK_EQ(render(std::multimap<int, int>{{1, 2}, {1, 3}}), "{(1, 2), (1, 3)}");
        CHECK_EQ(render(Mapping{}), "{(1, [2, 3])}");
        CHECK_EQ(render(std::vector<std::pair<int, int>>{{1, 2}}), "[(1, 2)]");
        CHECK_EQ(render(std::vector<std::vector<int>>{{}, {1, 2}}), "[[], [1, 2]]");
        CHECK_EQ(render(std::tuple<>{}), "()");
        CHECK_EQ(render(std::make_tuple(1, true, std::string("x"))), "(1, true, \"x\")");
        int array[2]{7, 8};
        CHECK_EQ(render(array), "[7, 8]");
        std::unordered_map<int, int> unordered{{2, 3}, {4, 5}};
        CHECK_EQ(render(unordered), plainMap(unordered));
    });
    runCase("DebugerHeadCopyToBitsStdC++/optional-and-variant", [] {
        CHECK_EQ(render(std::optional<int>{}), "nullopt");
        CHECK_EQ(render(std::optional<std::vector<int>>{std::vector<int>{1, 2}}), "optional([1, 2])");
        CHECK_EQ(render(std::variant<int, std::string>{7}), "variant(0: 7)");
        CHECK_EQ(render(std::variant<int, std::string>{std::string("x")}), "variant(1: \"x\")");
        std::variant<int, VariantFailure> value(7);
        throws<std::runtime_error>([&] { value.emplace<1>(true); });
        // Libraries may retain the old value or become valueless after a failed emplace.
        if (value.valueless_by_exception()) {
            CHECK_EQ(render(value), "valueless");
        } else {
            CHECK_EQ(render(value), "variant(0: 7)");
        }
    });
    runCase("DebugerHeadCopyToBitsStdC++/aggregates-and-fallbacks", [] {
        CHECK_EQ(render(A0{}), "{}");
        CHECK_EQ(render(A1{}), "{1}");
        CHECK_EQ(render(A2{}), "{1, 2}");
        CHECK_EQ(render(A3{}), "{1, 2, 3}");
        CHECK_EQ(render(A4{}), "{1, 2, 3, 4}");
        CHECK_EQ(render(A5{}), "{1, 2, 3, 4, 5}");
        CHECK_EQ(render(A6{}), "{1, 2, 3, 4, 5, 6}");
        CHECK_EQ(render(A7{}), "{1, 2, 3, 4, 5, 6, 7}");
        CHECK_EQ(render(A8{}), "{1, 2, 3, 4, 5, 6, 7, 8}");
        CHECK_EQ(render(A9{}), "<unprintable>");
        CHECK_EQ(render(Union{}), "<unprintable>");
        CHECK_EQ(render(Unknown{}), "<unprintable>");
        WithOwned value{Owned(7), 2};
        CHECK_EQ(render(value), "{owned(7), 2}");
    });
    runCase("DebugerHeadCopyToBitsStdC++/custom-output-priority", [] {
        CHECK_EQ(render(Priority{}), "preferred");
        _hashmap::Impl<int, 7, 8> map;
        map[1] = 2;
        CHECK_EQ(render(map), "custom-map");
        CHECK(map(1) == 2);
    });
    runCase("DebugerHeadCopyToBitsStdC++/macro-expressions", [] {
        configure();
        int count = 0;
        const int line = __LINE__ + 1;
        auto text = logged([&] { debug(++count); });
        std::ostringstream expected;
        expected << std::setw(4) << line << ": ++count = 1\n";
        CHECK_EQ(text, expected.str());
        CHECK(count == 1);
        CHECK_EQ(logged([] { debuger::log(10, "", std::tuple<>{}); }), "  10:  = ()\n");
        const int emptyLine = __LINE__ + 1;
        auto empty = logged([] { debug(); });
        std::ostringstream emptyExpected;
        emptyExpected << std::setw(4) << emptyLine << ":\n";
        CHECK_EQ(empty, emptyExpected.str());
        auto complex = logged([] { debug((1 < 2), (std::pair<int, int>{3, 4}), "a,b"); });
        CHECK(complex.find(": (1 < 2), (std::pair<int, int>{3, 4}), \"a,b\" = true, (3, 4), \"a,b\"\n") != complex.npos);
    });
    runCase("DebugerHeadCopyToBitsStdC++/color-and-spacing", [] {
        // FNV-1a("x") = 4245442695: green, three groups of four spaces.
        configure("color:true, space:true");
        CHECK_EQ(logged([] { debuger::log(10, "x", true); }),
                 std::string("\033[32m") + std::string(12, ' ') + "  10: x = true\033[0m\n");
        configure("color:false, space:true");
        CHECK_EQ(logged([] { debuger::log(10, "x", true); }), std::string(12, ' ') + "  10: x = true\n");
    });
    runCase("DebugerHeadCopyToBitsStdC++/cerr-state", [] {
        configure();
        const auto flags = std::cerr.flags();
        const auto precision = std::cerr.precision();
        const auto width = std::cerr.width();
        const auto fill = std::cerr.fill();
        std::cerr << std::hex << std::uppercase << std::scientific;
        std::cerr.precision(2); std::cerr.width(9); std::cerr.fill('_');
        const auto modified = std::cerr.flags();
        auto text = logged([] { debuger::log(12, "x", 1.5); });
        const bool kept = std::cerr.flags() == modified and std::cerr.precision() == 2 and
                          std::cerr.width() == 9 and std::cerr.fill() == '_';
        std::cerr.flags(flags); std::cerr.precision(precision); std::cerr.width(width); std::cerr.fill(fill);
        CHECK(kept);
        CHECK_EQ(text, "  12: x = 1.500000\n");
    });
    runCase("DebugerHeadCopyToBitsStdC++/reentry-and-exception-recovery", [] {
        configure();
        int count = 0;
        CHECK_EQ(logged([&] { debuger::log(12, "nested", Nested{&count}); }), "  12: nested = outer\n");
        CHECK(count == 1 and not debuger::busy());
        throws<std::runtime_error>([] { logged([] { debuger::log(12, "bad", Broken{}); }); });
        CHECK(not debuger::busy());
        CHECK_EQ(logged([] { debuger::log(12, "ok", 7); }), "  12: ok = 7\n");
    });
    runCase("DebugerHeadCopyToBitsStdC++/multi-tu-and-include-order", [] {
        configure("precision:7");
        CHECK(otherConfig() == &debuger::cfg());
        CHECK(otherBusy() == &debuger::busy());
        CHECK_EQ(logged([] { otherLog(1.5); }), "   9: value = 1.5000000\n");
        CHECK_EQ(otherMaps(), "{(2, 7)}|{(3, 9)}|{(4, 11)}");
    });
    runCase("DebugerHeadCopyToBitsStdC++/hashmap-empty-and-owners", [] {
        _hashmap::Impl<int, 1, 24> a, b;
        CHECK_EQ(render(a), "{}");
        a[1] = 10; b[1] = 99; a[2] = 20; b[3] = 30;
        CHECK_EQ(render(std::as_const(a)), "{(1, 10), (2, 20)}");
        CHECK_EQ(render(b), "{(1, 99), (3, 30)}");
        CHECK(a(1) == 10 and a(2) == 20 and b(1) == 99);
        a.clear(); CHECK_EQ(render(a), "{}");
        a[9] = 90;
        auto moved = std::move(a);
        CHECK_EQ(render(a), "{}");
        CHECK_EQ(render(moved), "{(9, 90)}");
        CHECK_EQ(render(b), "{(1, 99), (3, 30)}");
    });
    runCase("DebugerHeadCopyToBitsStdC++/hashmap-wide-key-and-full-pool", [] {
        _hashmap::Impl<int, 1, 4> map;
        map[0] = 0; map[std::numeric_limits<u64>::max()] = -1; map[2] = 4; map[3] = 9;
        CHECK_EQ(render(map), "{(0, 0), (18446744073709551615, -1), (2, 4), (3, 9)}");
        CHECK(map(std::numeric_limits<u64>::max()) == -1);
        CHECK_EQ(render(map), render(std::as_const(map)));
    });
    runCase("DebugerHeadCopyToBitsStdC++/hashmap-random-oracle", [] {
        for (int trial = 0; trial < 8; ++trial) {
            test_context::step = trial;
            _hashmap::Impl<int, 17, 64> map;
            std::map<u64, int> expected;
            for (int step = 0; step < 24; ++step) {
                u64 key = testRng() % 8;
                int value = randomInt(-99, 99);
                map[key] = value; expected[key] = value;
                auto text = render(map);
                test_context::input = text;
                CHECK(parseMap(text) == expected);
                for (auto [k, v] : expected) CHECK(map(k) == v);
                CHECK_EQ(render(map), text);
            }
        }
    });
    runCase("DebugerHeadCopyToBitsStdC++/hashmap-copy-contract", [] {
        _hashmap::Impl<Copies, 17, 8> map;
        map[1].value = 7; map[2].value = 9;
        Copies::count = 0;
        CHECK_EQ(render(map), "{(1, 7), (2, 9)}");
        CHECK(Copies::count == 2);
        CHECK(map(1).value == 7 and map(2).value == 9);
    });
    runCase("DebugerHeadCopyToBitsStdC++/treemap-order-and-state", [] {
        TreeMap<int, int, std::greater<int>> map;
        std::map<int, int, std::greater<int>> expected;
        CHECK_EQ(render(map), "{}");
        for (int step = 0; step < 32; ++step) {
            test_context::step = step;
            int key = randomInt(-8, 8), value = randomInt(-99, 99);
            if (step % 3 == 0) { map.erase(key); expected.erase(key); }
            else { map.insertOrAssign(key, value); expected[key] = value; }
            CHECK_EQ(render(std::as_const(map)), plainMap(expected));
            CHECK(map.size() == int(expected.size()));
            for (auto [k, v] : expected) CHECK(map(k) == v);
        }
        map.clear(); CHECK_EQ(render(map), "{}");
        map.insert(3, 7);
        auto moved = std::move(map);
        CHECK_EQ(render(map), "{}"); CHECK_EQ(render(moved), "{(3, 7)}");
    });
    runCase("DebugerHeadCopyToBitsStdC++/treemapoff-registration-and-bool", [] {
        TreeMapOff<int, bool, std::greater<int>> map({1, 3, 2, 1});
        CHECK_EQ(render(map), "{}");
        map[1] = true; map[3] = false;
        CHECK_EQ(render(std::as_const(map)), "{(3, false), (1, true)}");
        CHECK(map.size() == 2 and map.contains(1) and not map.contains(2));
        CHECK(map.rankOf(1) == 1);
        map.erase(3); CHECK_EQ(render(map), "{(1, true)}");
        map.clear(); CHECK_EQ(render(map), "{}");
        map[2] = false; CHECK_EQ(render(map), "{(2, false)}");
        auto moved = std::move(map);
        CHECK_EQ(render(map), "{}"); CHECK_EQ(render(moved), "{(2, false)}");
    });
    runCase("DebugerHeadCopyToBitsStdC++/treemap-values-without-copies", [] {
        TreeMap<int, Owned> map;
        map.insert(2, Owned(7)); map.insert(1, Owned(9));
        CHECK_EQ(render(map), "{(1, owned(9)), (2, owned(7))}");
        CHECK(map[1].value == 9 and map[2].value == 7);
        TreeMapOff<int, Owned> offline({1, 2});
        offline.insert(1, Owned(5));
        CHECK_EQ(render(offline), "{(1, owned(5))}");
        CHECK(offline[1].value == 5);
        TreeMap<int, Copies> copies;
        copies[1].value = 7;
        Copies::count = 0;
        CHECK_EQ(render(copies), "{(1, 7)}");
        CHECK(Copies::count == 0);
    });
    runCase("DebugerHeadCopyToBitsStdC++/nested-map-values", [] {
        TreeMap<int, std::vector<std::optional<int>>> tree;
        tree[1] = {std::nullopt, 7};
        CHECK_EQ(render(tree), "{(1, [nullopt, optional(7)])}");
        std::map<int, std::vector<std::optional<int>>> standard{{1, {std::nullopt, 7}}};
        CHECK_EQ(render(tree), render(standard));
        _hashmap::Impl<std::vector<int>, 17, 8> hash;
        hash[2] = {3, 4};
        CHECK_EQ(render(hash), "{(2, [3, 4])}");
        std::vector<TreeMap<int, int>> trees(2);
        trees[1][3] = 9;
        CHECK_EQ(render(trees), "[{}, {(3, 9)}]");
    });
    return 0;
}
