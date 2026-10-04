#pragma once
#include <cstdio>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

// SNIPPET BEGIN
class QInput {
    static constexpr int BufferSize = 1 << 20;
    FILE *file;
    char buffer[BufferSize + 1];
    char *pos = buffer;
    char *end = buffer;
    bool ended = false, failed = false;

    static bool space(unsigned char c) noexcept { return c == ' ' || unsigned(c - '\t') < 5; }

    bool refill() noexcept {
        if (ended || failed) return false;
        pos = buffer;
        end = buffer + std::fread(buffer, 1, BufferSize, file);
        *end = 0; // A non-digit sentinel lets the integer loop omit bounds checks.
        if (std::ferror(file)) {
            failed = true;
            return false;
        }
        ended = pos == end;
        return !ended;
    }

    bool ready() noexcept { return pos != end || refill(); }

    bool skip() noexcept {
        if (failed) return false;
        for (;;) {
            while (space(*pos))
                ++pos;
            if (pos != end) return true;
            if (!refill()) {
                failed = true;
                return false;
            }
        }
    }

    template <class U, class T>
    QInput &readInt(T &value) noexcept {
        if (!skip()) return *this;
        bool negative = *pos == '-';
        if (negative || *pos == '+') {
            ++pos;
            if (!ready()) {
                failed = true;
                return *this;
            }
        }
        if (unsigned(*pos - '0') > 9) {
            failed = true;
            return *this;
        }
        U x = 0;
        while (true) {
            while (unsigned(pos[0] - '0') < 10 && unsigned(pos[1] - '0') < 10) {
                x = U(x * 100 + unsigned(pos[0] - '0') * 10 + unsigned(pos[1] - '0'));
                pos += 2;
            }
            if (unsigned(*pos - '0') < 10) x = U(x * 10 + unsigned(*pos++ - '0'));
            if (pos != end || !refill()) break;
        }
        if (!failed) value = T(negative ? U(0) - x : x); // GCC: also handles the signed minimum.
        return *this;
    }

public:
    explicit QInput(FILE *source = stdin) noexcept : file(source) { buffer[0] = 0; }

    QInput(const QInput &) = delete;
    QInput &operator=(const QInput &) = delete;

    static QInput &shared() {
        static QInput input;
        return input;
    }

    explicit operator bool() const noexcept { return !failed; }

    bool fail() const noexcept { return failed; }

    bool eof() const noexcept { return ended; }

    void clear() noexcept {
        failed = ended = false;
        std::clearerr(file);
    }

    void tie(std::nullptr_t) noexcept {}

    void setFail() noexcept { failed = true; }

    QInput &operator>>(bool &x) noexcept {
        unsigned v;
        readInt<unsigned>(v);
        if (!failed) {
            failed = v > 1;
            if (!failed) x = bool(v);
        }
        return *this;
    }

    QInput &operator>>(short &x) noexcept { return readInt<unsigned short>(x); }

    QInput &operator>>(unsigned short &x) noexcept { return readInt<unsigned short>(x); }

    template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    QInput &operator>>(T &value) {
        std::string token;
        if (!(*this >> token)) return *this;
        char *end = nullptr;
        errno = 0;
        T number;
        if constexpr (std::is_same_v<T, float>)
            number = std::strtof(token.c_str(), &end);
        else if constexpr (std::is_same_v<T, double>)
            number = std::strtod(token.c_str(), &end);
        else
            number = std::strtold(token.c_str(), &end);
        bool invalid = end == token.c_str() || end != token.c_str() + token.size() ||
                       (errno == ERANGE && (number == 0 || !std::isfinite(number)));
        if (invalid)
            failed = true;
        else
            value = number;
        return *this;
    }

    QInput &operator>>(int &x) noexcept { return readInt<unsigned>(x); }

    QInput &operator>>(unsigned &x) noexcept { return readInt<unsigned>(x); }

    QInput &operator>>(long &x) noexcept { return readInt<unsigned long>(x); }

    QInput &operator>>(unsigned long &x) noexcept { return readInt<unsigned long>(x); }

    QInput &operator>>(long long &x) noexcept { return readInt<unsigned long long>(x); }

    QInput &operator>>(unsigned long long &x) noexcept { return readInt<unsigned long long>(x); }

    QInput &operator>>(__int128 &x) noexcept { return readInt<unsigned __int128>(x); }

    QInput &operator>>(unsigned __int128 &x) noexcept { return readInt<unsigned __int128>(x); }

    QInput &operator>>(char &c) noexcept {
        if (skip()) c = *pos++;
        return *this;
    }

    QInput &operator>>(std::string &text) {
        if (!skip()) return *this;
        text.clear();
        do {
            const char *first = pos;
            while (pos != end && !space(*pos))
                ++pos;
            text.append(first, pos - first);
            if (pos != end) break;
        } while (refill());
        return *this;
    }

    QInput &getLine(std::string &text, char delimiter = '\n') {
        if (failed) return *this;
        text.clear();
        while (ready()) {
            char *stop = static_cast<char *>(std::memchr(pos, delimiter, end - pos));
            if (stop) {
                text.append(pos, stop - pos);
                pos = stop + 1;
                return *this;
            }
            text.append(pos, end - pos);
            pos = end;
        }
        if (text.empty()) failed = true;
        return *this;
    }
};

class QOutput {
    static constexpr int BufferSize = 1 << 20;
    FILE *file;
    char buffer[BufferSize];
    int used = 0;
    bool failed = false;

    void drain() noexcept {
        if (used && std::fwrite(buffer, 1, used, file) != unsigned(used)) failed = true;
        used = 0;
    }

    template <class U, class T>
    QOutput &writeInt(T value) noexcept {
        U x = U(value);
        bool negative = value < 0;
        if (negative) x = U(0) - x;
        char digits[sizeof(T) * 3 + 1];
        char *pos = digits + sizeof(digits);
        static const char pairs[] = "0001020304050607080910111213141516171819"
                                    "2021222324252627282930313233343536373839"
                                    "4041424344454647484950515253545556575859"
                                    "6061626364656667686970717273747576777879"
                                    "8081828384858687888990919293949596979899";
        while (x >= 100) {
            unsigned r = unsigned(x % 100);
            x /= 100;
            pos -= 2;
            std::memcpy(pos, pairs + r * 2, 2);
        }
        if (x < 10)
            *--pos = char('0' + x);
        else {
            pos -= 2;
            std::memcpy(pos, pairs + unsigned(x) * 2, 2);
        }
        if (negative) *--pos = '-';
        return write(pos, digits + sizeof(digits) - pos);
    }

public:
    explicit QOutput(FILE *target = stdout) noexcept : file(target) {}

    QOutput(const QOutput &) = delete;
    QOutput &operator=(const QOutput &) = delete;

    ~QOutput() { flush(); }

    static QOutput &shared() {
        static QOutput output;
        return output;
    }

    explicit operator bool() const noexcept { return !failed; }

    bool fail() const noexcept { return failed; }

    QOutput &flush() noexcept {
        drain();
        if (std::fflush(file) != 0) failed = true;
        return *this;
    }

    QOutput &write(const char *data, std::size_t size) noexcept {
        while (size && !failed) {
            if (!used && size >= BufferSize) {
                if (std::fwrite(data, 1, size, file) != size) failed = true;
                break;
            }
            std::size_t room = BufferSize - used;
            std::size_t count = size < room ? size : room;
            std::memcpy(buffer + used, data, count);
            used += int(count);
            data += count;
            size -= count;
            if (used == BufferSize) drain();
        }
        return *this;
    }

    QOutput &operator<<(bool x) noexcept { return *this << char('0' + x); }

    QOutput &operator<<(short x) noexcept { return writeInt<unsigned short>(x); }

    QOutput &operator<<(unsigned short x) noexcept { return writeInt<unsigned short>(x); }

    QOutput &operator<<(int x) noexcept { return writeInt<unsigned>(x); }

    QOutput &operator<<(unsigned x) noexcept { return writeInt<unsigned>(x); }

    QOutput &operator<<(long x) noexcept { return writeInt<unsigned long>(x); }

    QOutput &operator<<(unsigned long x) noexcept { return writeInt<unsigned long>(x); }

    QOutput &operator<<(long long x) noexcept { return writeInt<unsigned long long>(x); }

    QOutput &operator<<(unsigned long long x) noexcept { return writeInt<unsigned long long>(x); }

    QOutput &operator<<(__int128 x) noexcept { return writeInt<unsigned __int128>(x); }

    QOutput &operator<<(unsigned __int128 x) noexcept { return writeInt<unsigned __int128>(x); }

    QOutput &operator<<(char c) noexcept {
        if (!failed) {
            buffer[used++] = c;
            if (used == BufferSize) drain();
        }
        return *this;
    }

    QOutput &operator<<(const char *text) noexcept { return write(text, std::strlen(text)); }

    QOutput &operator<<(const std::string &text) noexcept {
        return write(text.data(), text.size());
    }

    QOutput &operator<<(std::string_view text) noexcept { return write(text.data(), text.size()); }

    template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    QOutput &operator<<(T value) {
        return writeReal(value, 6);
    }

    template <class T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
    QOutput &writeReal(T value, int precision) {
        if (precision < 0) {
            failed = true;
            return *this;
        }
        char local[128];
        int length = std::snprintf(local, sizeof(local), "%.*Lf", precision,
                                   static_cast<long double>(value));
        if (length < 0) {
            failed = true;
            return *this;
        }
        if (std::size_t(length) < sizeof(local)) return write(local, std::size_t(length));
        std::vector<char> text(std::size_t(length) + 1);
        std::snprintf(text.data(), text.size(), "%.*Lf", precision,
                      static_cast<long double>(value));
        return write(text.data(), std::size_t(length));
    }

    QOutput &operator<<(std::ostream &(*fn)(std::ostream &)) noexcept {
        if (fn == std::endl<char, std::char_traits<char>>) {
            *this << '\n';
            flush();
        } else if (fn == std::flush<char, std::char_traits<char>>)
            flush();
        else if (fn == std::ends<char, std::char_traits<char>>)
            *this << '\0';
        else
            failed = true;
        return *this;
    }
};

// Old spellings remain aliases; explicit streams avoid rewriting std::cin/std::cout.
using Qinput = QInput;
using Qoutput = QOutput;

inline QInput &fastIn() {
    return QInput::shared();
}

inline QOutput &fastOut() {
    return QOutput::shared();
}
