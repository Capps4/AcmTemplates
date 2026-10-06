# 1. Math
## MathPackage
### Sieve
<!-- @code Sieve -->

### ModuloInteger
<!-- @code ModuloInteger -->

```plain
V <= 1e9 : 1004535809
V <= 1e15 : 1337006139375617
V <= 4e18 : 4179340454199820289
```

### Combinatorics
<!-- @code Combinatorics -->

### FloatPointNumber
<!-- @code FloatPointNumber -->

### ExGcd 
<!-- @code ExGcd -->

For the equation$ ax+by=c $, invoking exgcd, get $ x_0 $and$ y_0 $ such that $ ax_0+by_0=\gcd(a, b) $.

When $ \gcd(a,b)\mid c $ the general solution is :

$ x = x_0 \times \frac{c}{\gcd(a, b)}+k\times \frac{b}{\gcd(a, b)}
\\ \ 
\\  \\
y = y_0 \times \frac{c}{\gcd(a, b)}-k\times \frac{a}{\gcd(a, b)}
 $

## Polynomial
### FastFourierTransform
<!-- @code FastFourierTransform -->

### NumberTheoreticTransform
<!-- @code NumberTheoreticTransform -->

## Linear Algebra
### GaussianElimination
<!-- @code GaussianElimination -->

### LinearBasis
<!-- @code LinearBasis -->

## Random Number Algorithm
### RandomNumber
<!-- @code RandomNumber -->

### MillerRabin
<!-- @code MillerRabin -->

### PollardRho
<!-- @code PollardRho -->

If$ n $is a prime number (by MillerRabbin) , return$ n $,

otherwise return a random factor of$ n $in$ [2,n-1] $.

Time complexity:$ O(n^{\frac{1}{4}}\log n) $, but fast as$ O(n^{\frac{1}{4}}) $.

# 2. Dynamic Programming
## MultipleBackpacks
<!-- @code MultipleBackpacks -->

# 3. Sorting
## CountingSortOrder
<!-- @code CountingSortOrder -->

## Discreter
<!-- @code Discreter -->

## MoSort
<!-- @code MoSort -->

In mo processing, use #define add(x) & del(x) for atomic operator.

# 4. String
## String F4
### StringHash
<!-- @code StringHash -->

### Kmp
<!-- @code Kmp -->

### Z-Function
<!-- @code ZFunction -->

### Manacher
<!-- @code Manacher -->

### MinimalString
<!-- @code MinimalString -->

## Advanced Strings
### AcAutomaton
<!-- @code AcAutomaton -->

### SuffixAutomaton
<!-- @code SuffixAutomaton -->

### ExSuffixAutomaton
<!-- @code ExSuffixAutomaton -->

### PalindromeAutomaton
<!-- @code PalindromeAutomaton -->

### SuffixArray
<!-- @code SuffixArray -->



# 5. DataStructures
## BaseDataStructures
### HashMap
<!-- @code HashMap -->

### DisjointSetUnion
<!-- @code DisjointSetUnion -->

### RMQ
<!-- @code RMQ -->

### DeletableHeap
<!-- @code DeletableHeap -->

## TreeDataStructures
### TreeMap
<!-- @code TreeMap -->

### SegTree
<!-- @code SegTree -->

### Trie
<!-- @code Trie -->

### FenwickTree
<!-- @code FenwickTree -->

### SparseSegTree
<!-- @code SparseSegTree -->

# 6. Tree Theory
## Tree
<!-- @code Tree -->

## CentroidDecomposition
<!-- @code CentroidDecomposition -->

# 7. Graph Theory
## GridUtil
<!-- @code GridUtil -->

## Dijkstra
<!-- @code Dijkstra -->

## RingTree
<!-- @code RingTree -->

## TopSort
<!-- @code TopSort -->

## Connectivity
### StronglyConnectedComponent
<!-- @code StronglyConnectedComponent -->

### TwoSat
<!-- @code TwoSat -->

### VertexBiconnectedComponent
<!-- @code VertexBiconnectedComponent -->

### EdgeBiconnectedComponent
<!-- @code EdgeBiconnectedComponent -->

## Flow
### MaxFlow
<!-- @code MaxFlow -->

In a bipartite graph with $ n $ vertices and $ m $ edges:

+ Maximum matching $ = $ Minimum vertex cover
+ Maximum independent set $ = $ $ n $ - Minimum vertex cover

After running the max flow work:

1. $ S $ is the set that can be reached from start point in the residual network
2. All unmatched vertices in the left partition $ \in S $, and all unmatched vertices in the right partition $ \notin S $.
3. For a matched edge, either both endpoints are $ \in S $, or both are $ \notin S $.

**Construct the minimum cut:**

+ The minimum cut = $ \{ $** **$ x \rightarrow y $** **$ | $** **$ x\in S \wedge y \notin S $** **$ \} $.

**Construct the minimum vertex cover:**

+ The minimum vertex cover $ = $ $ \{ $**vertices in the left and **$ \notin S $** **$ \} $**  **$ \cup $**  **$ \{ $**vertices in the right and **$ \in S $$ \} $.

**Construct the maximum independent set:**

+ The maximum independent set $ = $ **all vertices** $ - $ **The minimum vertex cover**

**Construct the longest antichain in a DAG **(A max set of mutually unreachable vertives) (min unoverlapping chains coverage):

+ First, split the vertices of the DAG to a bipartite graph, if $ x \rightarrow y $ in DAG, let $ x_l\rightarrow y_r $ in bipartite graph, then run the maximum matching (maxFlow) in the bipartite graph.
+ The longest antichain $ = $ $ \{ $** **$ x $** **$ | $** **$ x_l $** and **$ x_r $** both **$ \in $** maximum independent set **$ \} $

### CostFlow
<!-- @code CostFlow -->

This minimum cost flow doesn't support negative cost **cycles**. However, meeting negative cost cycles are extremely rare, and it can handle negative **edge** weights.

# 8. Geometry
## Geo2
### PointVec
<!-- @code Geo2/PointVec -->

### SegLine
<!-- @code Geo2/SegLine -->

### PolygonConvex
<!-- @code Geo2/PolygonConvex -->

### Circle
<!-- @code Geo2/Circle -->

## Geo3
<!-- @code Geo3 -->

# 9. Clarketech
### ListHelper
<!-- @code ListHelper -->

### Debuger
<!-- @code Debuger -->

### DebugerHeadCopyToBitsStdC++
```c
/*
# **竞赛 输出调试工具（COMPETITION_DEBUGER）README**

## 前置条件
要求编译C++版本至少为C++11

## 用法

1. 将此代码贴入万能头内部最末尾，一定是最末尾哦

2. 然后在需要使用debug的文件前面贴入如下四行：
auto $ = strdup("color: false, space: false, precision: 6");
#ifndef COMPETITION_DEBUGER
    #define debug(...)
#endif

** (注意，不贴这几行直接使用debug()会运行时报错) **

## 效果
然后就可以随意debug各种类型了
比如 int, double, pair, tuple, vector, set, array, 指针等
还支持多参数调用，比如 debug(a, b, c, d);
**C++17及以上还能支持输出自定义简单struct**

## 额外配置

在strdup的参数里做配置：
设置color为true 可解锁终端颜色输出
 - 对相同变量名采用同一颜色，不同变量名采用不同颜色
 - 在使用vscode+cph时不建议开启，终端输出用户建议开启

设置space为true 可解锁前置空格
 - 对相同变量名采用同长度前置空格，不同变量名采用不同前置空格

（上述二者都是为了方便查看变量）

设置precision的值可以调整debug浮点数的精度输出

## 额外注意
代码里不能再使用strdup函数了，如果不认识这个函数那就无所谓了
想了解可以：https://blog.csdn.net/weixin_44498318/article/details/116082649

如果希望debug() 空表达式只输出行号，请将编译参数从 -std=c++xx 改成 -std=gnu++xx

** 代码末尾配赠了单例测试

*/
#ifndef COMPETITION_DEBUGER
#define COMPETITION_DEBUGER

namespace _$competition_debug {
    #if __cplusplus >= 201703L
    // Detail: Helpers for aggregate struct processing
    namespace detail_for_aggregate_struct {
        // Helper type for aggregate field count deduction (implicitly convertible to any type)
        struct any_type_helper {
            template<typename T>
            operator T();
        };

        // SFINAE: Check if T can be constructed with Args
        template <typename T, typename... Args>
        struct can_construct_with_args {
        private:
            template <typename... A>
            static auto test(int) -> decltype(T{std::declval<A>()...}, std::true_type{});
            template <typename... A>
            static std::false_type test(...);
        public:
            static constexpr bool value = decltype(test<Args...>(0))::value;
        };

        // Compile-time: Get field count of aggregate type
        template<typename T, typename... Args>
        constexpr auto get_aggregate_field_count() {
            if constexpr (!can_construct_with_args<T, Args...>::value) {
                return sizeof...(Args) - 1; // Return last valid count when construction fails
            } else {
                return get_aggregate_field_count<T, Args..., any_type_helper>();
            }
        }

        // Trait: Check if T is custom aggregate (needs special print)
        template <typename T, typename = void>
        struct is_custom_aggregate_type : std::false_type {};

        template <typename T>
        struct is_custom_aggregate_type<T, std::void_t<
            std::enable_if_t<std::is_aggregate_v<T>>,        // Must be aggregate
            std::enable_if_t<!std::is_fundamental_v<T>>,      // Not fundamental type
            std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::string>>, // Not string
            std::enable_if_t<!std::is_array_v<T>>             // Not C-array
        >> : std::true_type {};

        // Convert aggregate type to tuple (max 8 fields supported)
        template<typename T, std::enable_if_t<std::is_aggregate_v<std::decay_t<T>>, int> = 0>
        constexpr auto aggregate_to_tuple(const T& t) {
            using decay_t = std::decay_t<T>;
            constexpr auto field_count = get_aggregate_field_count<decay_t>();
            
            if constexpr (field_count == 1) {
                auto const& [_1] = t;
                return std::make_tuple(_1);
            } else if constexpr (field_count == 2) {
                auto const& [_1, _2] = t;
                return std::make_tuple(_1, _2);
            } else if constexpr (field_count == 3) {
                auto const& [_1, _2, _3] = t;
                return std::make_tuple(_1, _2, _3);
            } else if constexpr (field_count == 4) {
                auto const& [_1, _2, _3, _4] = t;
                return std::make_tuple(_1, _2, _3, _4);
            } else if constexpr (field_count == 5) {
                auto const& [_1, _2, _3, _4, _5] = t;
                return std::make_tuple(_1, _2, _3, _4, _5);
            } else if constexpr (field_count == 6) {
                auto const& [_1, _2, _3, _4, _5, _6] = t;
                return std::make_tuple(_1, _2, _3, _4, _5, _6);
            } else if constexpr (field_count == 7) {
                auto const& [_1, _2, _3, _4, _5, _6, _7] = t;
                return std::make_tuple(_1, _2, _3, _4, _5, _6, _7);
            } else if constexpr (field_count == 8) {
                auto const& [_1, _2, _3, _4, _5, _6, _7, _8] = t;
                return std::make_tuple(_1, _2, _3, _4, _5, _6, _7, _8);
            } else {
                // Error: Exceed max supported fields
                static_assert(field_count <= 8, "Custom aggregate has >8 fields. Extend aggregate_to_tuple.");
            }
        }
    } // end namespace detail_for_aggregate_struct
    #endif


    auto& debug_output_stream = std::cerr;
    const char* debug_interval_str = ", ";

    // Debug config flags
    bool output_color_enabled = false;
    bool output_indent_enabled = false;
    int debug_precision = 6;
    bool pointer_output_value = true;

    // Detail: Debug print implementation
    namespace implementation {

        // Util: check object can be printed
        template <typename T>
        struct is_printable {
        private:
            template <typename U>
            static auto test(U* u) -> decltype(std::cerr << *u, std::true_type());
            
            template <typename U>
            static std::false_type test(...);
            
        public:
            static constexpr bool value = decltype(test<T>(nullptr))::value;
        };
        
        #if __cplusplus >= 201703L
        template <class T>
        std::enable_if_t<!detail_for_aggregate_struct::is_custom_aggregate_type<T>::value, void>
        debug_print(const T& arg);

        template <class T>
        std::enable_if_t<detail_for_aggregate_struct::is_custom_aggregate_type<T>::value, void>
        debug_print(const T& arg);
        #else
        template <class T>
        typename std::enable_if<is_printable<T>::value>::type
        debug_print(const T& arg);

        template <class T>
        typename std::enable_if<!is_printable<T>::value>::type
        debug_print(const T& arg);
        #endif

        template <class T>
        void debug_print(T* arg);

        void debug_print(std::nullptr_t arg);

        void debug_print(const bool& arg);
        void debug_print(const float& arg);
        void debug_print(const double& arg);
        void debug_print(const long double& arg);

        void debug_print(const char& c);

        void debug_print(const std::string& arg);
        void debug_print(const char* arg);
        void debug_print(char* arg);

        template <class T, class K>
        void debug_print(const std::pair<T, K>& arg);

        template <class... Ts>
        void debug_print(const std::tuple<Ts...>& arg);

        template <class T>
        void debug_print(const std::vector<T>& arg);

        template <class T, std::size_t N>
        void debug_print(const std::array<T, N>& arg);

        template <class T>
        void debug_print(const std::deque<T>& arg);

        template <class T>
        void debug_print(const std::list<T>& arg);

        template <class T>
        void debug_print(const std::forward_list<T>& arg);

        template <class T>
        void debug_print(const std::multiset<T>& arg);

        template <class T>
        void debug_print(const std::set<T>& arg);

        template <class T>
        void debug_print(const std::unordered_set<T>& arg);

        template <class K, class V>
        void debug_print(const std::map<K, V>& arg);

        template <class K, class V>
        void debug_print(const std::unordered_map<K, V>& arg);

        #if __cplusplus >= 201703L
        // General version: For non-custom aggregate types
        template <class T>
        std::enable_if_t<!detail_for_aggregate_struct::is_custom_aggregate_type<T>::value, void>
        debug_print(const T& arg) {
            if constexpr (is_printable<T>::value) {
                debug_output_stream << arg;
            } else {
                debug_output_stream << "!unknow";
            }
        }

        // For custom aggregates: Convert to tuple first
        template <class T>
        std::enable_if_t<detail_for_aggregate_struct::is_custom_aggregate_type<T>::value, void>
        debug_print(const T& arg) {
            debug_print(detail_for_aggregate_struct::aggregate_to_tuple(arg));
        }

        #else
        template <class T>
        typename std::enable_if<is_printable<T>::value>::type
        debug_print(const T& arg) {
            debug_output_stream << arg;
        }
        
        template <class T>
        typename std::enable_if<!is_printable<T>::value>::type
        debug_print(const T& arg) {
            debug_output_stream << "!unknow";
        }
        #endif

        bool has_output_pointer = false;
        struct pointer_info_tips_t {
            pointer_info_tips_t() {}
            ~pointer_info_tips_t() {
                if (implementation::has_output_pointer) {
                    if (pointer_output_value) {
                        debug_output_stream << "\nSorry, I can't diff C-arrays and pointers";
                        if (pointer_output_value) {
                            debug_output_stream << ", you can set pointer_addr: true in $ to check addr";
                        }
                        debug_output_stream << ".\n";
                    }
                    implementation::has_output_pointer = false;
                }
            }
        };
        pointer_info_tips_t pointer_info_tips_util;

        // Pointer
        template <class T>
        void debug_print(T* arg) {
            has_output_pointer = true;
            if (!pointer_output_value) {
                debug_output_stream << static_cast<const void*>(arg);
            } else {
                if (arg != nullptr) {
                    debug_output_stream << "&";
                    debug_print(*arg);
                } else {
                    debug_print(nullptr);
                }
            }
        }

        // Print nullptr
        void debug_print(std::nullptr_t arg) {
            debug_output_stream << "nullptr";
        }

        // Print bool as "true"/"false"
        void debug_print(const bool& arg) {
            debug_output_stream << (arg ? "true" : "false");
        }

        // Macro: Print floating-point types with fixed precision
        #define debug_print_float(Tp) void debug_print(const Tp& arg) { \
            debug_output_stream << std::fixed << std::setprecision(debug_precision) << arg; \
        }
        debug_print_float(float)
        debug_print_float(double)
        debug_print_float(long double)
        #undef debug_print_float

        // Print std::pair (format: (a, b))
        template <class T, class K>
        void debug_print(const std::pair<T, K>& arg) {
            debug_output_stream << '(';
            debug_print(arg.first);
            debug_output_stream << debug_interval_str;
            debug_print(arg.second);
            debug_output_stream << ')';
        }

        // Print std::string (format: "str")
        void debug_print(const std::string& arg) {
            debug_output_stream << '\"';
            debug_output_stream << arg;
            debug_output_stream << '\"';
        }

        // Print C-string (wrap to std::string print)
        void debug_print(const char* arg) {
            debug_print(std::string(arg));
        }

        void debug_print(char *arg) {
            debug_print(std::string(arg));
        }

        // Print char (format: 'c')
        void debug_print(const char& c) {
            debug_output_stream << '\'';
            debug_output_stream << c;
            debug_output_stream << '\'';
        }

        // Detail: Helpers for std::tuple print
        namespace detail_for_tuple_print {
            template <typename Tuple, std::size_t N>
            struct tuple_print_helper {
                static void print(const Tuple& t) {
                    tuple_print_helper<Tuple, N-1>::print(t);
                    debug_output_stream << debug_interval_str;
                    implementation::debug_print(std::get<N-1>(t));
                }
            };

            template <typename Tuple>
            struct tuple_print_helper<Tuple, 1> {
                static void print(const Tuple& t) {
                    implementation::debug_print(std::get<0>(t));
                }
            };
        }

        // Print std::tuple (format: (a, b, c))
        template <class... Ts>
        void debug_print(const std::tuple<Ts...>& arg) {
            debug_output_stream << '(';
            detail_for_tuple_print::tuple_print_helper<decltype(arg), sizeof...(Ts)>::print(arg);
            debug_output_stream << ')';
        }

        // Print container elements (iter range)
        template <class Iterator>
        void debug_print_container_elements(Iterator first, Iterator last) {
            for (Iterator it = first; it != last; ++it) {
                if (it != first) {
                    debug_output_stream << debug_interval_str;
                }
                debug_print(*it);
            }
        }

        // Print C-array (format: [a, b, c])
        template <class T>
        void debug_print_array_elements(const T* arg, int N) {
            debug_output_stream << '[';
            debug_print_container_elements(arg, arg + N);
            debug_output_stream << ']';
        }

        // Print std::array
        template <class T, std::size_t N>
        void debug_print(const std::array<T, N>& arg) {
            debug_print_array_elements(arg.data(), arg.size());
        }

        // Macro: Print square-bracket containers (vector/deque/list)
        #define debug_print_square_container(ContainerType) template <class T> void debug_print(const ContainerType<T>& arg) { \
            debug_output_stream << '['; \
            debug_print_container_elements(arg.begin(), arg.end()); \
            debug_output_stream << ']'; \
        }
        debug_print_square_container(std::vector)
        debug_print_square_container(std::deque)
        debug_print_square_container(std::list)
        debug_print_square_container(std::forward_list)
        #undef debug_print_square_container
        
        // Macro: Print flower-bracket set-like containers
        #define debug_print_set_like_container(ContainerType) template <class T> void debug_print(const ContainerType<T>& arg) { \
            debug_output_stream << '{'; \
            debug_print_container_elements(arg.begin(), arg.end()); \
            debug_output_stream << '}'; \
        }
        debug_print_set_like_container(std::set)
        debug_print_set_like_container(std::multiset)
        debug_print_set_like_container(std::unordered_set)
        debug_print_set_like_container(std::unordered_multiset)
        #undef debug_print_set_like_container
        
        // Macro: Print flower-bracket map-like containers
        #define debug_print_map_like_container(ContainerType) template <class K, class V> void debug_print(const ContainerType<K, V>& arg) { \
            debug_output_stream << '{'; \
            debug_print_container_elements(arg.begin(), arg.end()); \
            debug_output_stream << '}'; \
        }
        debug_print_map_like_container(std::map)
        debug_print_map_like_container(std::unordered_map)
        debug_print_map_like_container(std::multimap)
        debug_print_map_like_container(std::unordered_multimap)
        #undef debug_print_map_like_container
    } // end namespace implementation

    // ANSI color codes for debug output
    const std::string debug_color_codes[] = {
        "\033[93m",   // Yellow
        "\033[94m",   // Blue
        "\033[96m",   // Cyan
        "\033[92m",   // Green
        "\033[95m"    // Red
    };
    const std::string color_reset_code = "\033[0m";

    // Get C-array size at compile-time
    template <class T, std::size_t N>
    std::size_t get_array_size(T (&)[N]) {
        return N;
    }

    // Apply debug color (if enabled)
    inline void set_debug_color(int color_idx) {
        if (output_color_enabled) {
            debug_output_stream << debug_color_codes[color_idx];
        }
    }

    // Reset color (if enabled)
    inline void reset_debug_color() {
        if (output_color_enabled) {
            debug_output_stream << color_reset_code;
        }
    }

    // Map: Debug variable name -> color index
    std::map<std::string, int> name_to_color_map;
    int current_color_index = 0;

    // Flag: Ensure debug is enabled before use
    bool debug_enabled_flag = false;

    // Core debug print (with line number/name/color)
    template <class T>
    void debug_print_with_meta(int line_num, const std::string& var_name, T const &arg) {
        assert(debug_enabled_flag && "Debug not enabled! Call _$competition_set_debug_config first.");

        // Assign color if var_name not in map
        if (name_to_color_map.count(var_name) == 0) {
            name_to_color_map[var_name] = current_color_index;
            current_color_index = (current_color_index + 1) % get_array_size(debug_color_codes);
        }
        int color_idx = name_to_color_map[var_name];

        // Add indent if enabled
        if (output_indent_enabled) {
            for (int i = 0; i < color_idx; ++i) {
                debug_output_stream << "  ";
            }
        }

        // Print meta info (line number + var name)
        reset_debug_color();
        set_debug_color(color_idx);
        debug_output_stream << std::setw(3) << std::to_string(line_num) << ": ";
        debug_output_stream << var_name << " = ";

        // Print variable value
        implementation::debug_print(arg);
        
        // Reset and flush
        debug_output_stream << '\n';
        reset_debug_color();
        debug_output_stream << std::flush;
    }

    // Trim whitespace from string (both ends)
    std::string trim_string(const std::string& s) {
        auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char c) {
            return std::isspace(c);
        });
        auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) {
            return std::isspace(c);
        }).base();
        return (start < end) ? std::string(start, end) : "";
    }

    // Set debug config (format: "color:true, space:true, precision:9")
    const char *_$competition_set_debug_config(const char* config_str) {
        debug_enabled_flag = true;
        std::string config = std::string(config_str);
        
        // Convert config to lowercase for case-insensitive parsing
        for (char& c : config) {
            if (std::isupper(static_cast<unsigned char>(c))) {
                c = std::tolower(static_cast<unsigned char>(c));
            }
        }

        // Split config by comma
        std::istringstream config_stream(config);
        std::string key_value_pair;
        while (std::getline(config_stream, key_value_pair, ',')) {
            // Split key-value by colon
            std::istringstream kv_stream(key_value_pair);
            std::string key, value;
            if (std::getline(kv_stream, key, ':') && std::getline(kv_stream, value)) {
                key = trim_string(key);
                value = trim_string(value);

                // Update config based on key
                if (key == "color") {
                    output_color_enabled = (value == "true");
                } else if (key == "space") {
                    output_indent_enabled = (value == "true");
                } else if (key == "precision") {
                    debug_precision = std::stoi(value);
                } else if (key == "pointer_addr") {
                    pointer_output_value = (value != "true");
                }
            }
        }
        return "";
    }

    // Find first separator (,/\0) outside nested brackets
    const char* find_first_separator(const char* str) {
        const int max_scan_limit = 1024;
        if (!str) return nullptr;
        
        int bracket_depth = 0;  // ()
        int angle_depth = 0;    // <>
        int brace_depth = 0;    // {}
        int square_depth = 0;   // []
        int scan_count = 0;

        while (scan_count < max_scan_limit) {
            switch (*str) {
                case '<': angle_depth++; break;
                case '>': angle_depth--; break;
                case '(': bracket_depth++; break;
                case ')': bracket_depth--; break;
                case '{': brace_depth++; break;
                case '}': brace_depth--; break;
                case '[': square_depth++; break;
                case ']': square_depth--; break;
                default: break;
            }

            // Check if current char is valid separator
            if ((*str == ',' || *str == '\0') && 
                bracket_depth == 0 && angle_depth == 0 && 
                brace_depth == 0 && square_depth == 0) {
                return str;
            }

            str++;
            scan_count++;
        }
        return nullptr;
    }

    // Count number of separators in expr string
    size_t count_separators(const char* str) {
        size_t count = 0;
        while (true) {
            const char* separator = find_first_separator(str);
            if (!separator) return static_cast<size_t>(-1); // Invalid format
            
            count++;
            if (*separator == '\0') break;
            str = separator + 1;
        }
        return count;
    }

    // Base case: No more args to print
    void _$debug_print_dispatch(int line, const char* str) {
        assert(str != nullptr && "Empty expression string!");
        assert(*str == '\0' && "Mismatched args and expressions!");
        debug_output_stream << std::setw(3) << std::to_string(line) << ":\n";
        return;
    }

    // Recursive case: Print first arg, then rest
    template<typename FirstArg, typename... RestArgs>
    void _$debug_print_dispatch(int line_num, const char* expr_str, FirstArg const &first_val, RestArgs const &... rest_vals) {
        const size_t rest_arg_count = sizeof...(RestArgs);
        assert(count_separators(expr_str) == rest_arg_count + 1 && "Arg count != expression count!");

        // Extract current expression (before first separator)
        const char* separator = find_first_separator(expr_str);
        assert(separator != nullptr && "Invalid expression format!");
        std::string current_expr = trim_string(std::string(expr_str, separator));

        // Print current arg with meta info
        debug_print_with_meta(line_num, current_expr, first_val);

        // Recurse for rest args
        if (rest_arg_count > 0) {
            _$debug_print_dispatch(line_num, separator + 1, rest_vals...);
        }
    }
} // end namespace competition_debug

// Export core functions to global namespace
using _$competition_debug::_$competition_set_debug_config;
using _$competition_debug::_$debug_print_dispatch;

// Debug macro: Auto pass line number + expression string + args
#define debug(...) _$debug_print_dispatch(__LINE__, #__VA_ARGS__, ##__VA_ARGS__)

// Compatibility macro: Map old "strdup" to config function
#define strdup _$competition_set_debug_config

#endif


/*
**********  单例测试  *********


#include <bits/stdc++.h>

auto $ = strdup("color: true, space: false, precision: 6");
#ifndef COMPETITION_DEBUGER
    #define debug(...)
#endif


// 测试用自定义聚合类型（结构体，最多8个字段，符合工具限制）
struct Point2D { int x; int y; };                  // 2字段
struct Student { std::string name; int age; float score; }; // 3字段
struct Rectangle { int left; int top; int right; int bottom; }; // 4字段
struct Line2D {
    Point2D p1;
    Point2D p2;
};

// 单例测试类（确保全局唯一实例，集中测试所有数据类型）
class DebugTester {
public:
    // 单例模式：获取唯一实例
    static DebugTester& getInstance() {
        static DebugTester instance; // 静态局部变量，保证线程安全（C++11及以上）
        return instance;
    }

    // 禁止拷贝和赋值（确保单例唯一性）
    DebugTester(const DebugTester&) = delete;
    DebugTester& operator=(const DebugTester&) = delete;

    // 测试1：基础数据类型（int、double、char、bool等）
    void testBasicTypes() {
        std::cerr << "\n===== 测试1：基础数据类型 =====" << std::endl;
        int a = 1024;                  // 整型
        double b = 3.1415926535;       // 双精度浮点
        float c = 2.71828f;            // 单精度浮点
        char d = 'A';                  // 字符
        bool e = true;                 // 布尔值
        long long f = 9223372036854775807; // 长整型
        unsigned int g = 4294967295;   // 无符号整型

        // 多参数debug调用（最多支持8个参数）
        debug(a, b, c, d, e, f, g);
    }

    // 测试2：STL容器类型（vector、set、map等）
    void testSTLContainers() {
        std::cerr << "\n===== 测试2：STL容器类型 =====" << std::endl;
        // 1. 顺序容器
        std::vector<int> vec = {1, 2, 3, 4, 5};          // 动态数组
        std::array<int, 3> arr = {10, 20, 30};           // 固定大小数组
        std::deque<int> deq = {100, 200, 300};           // 双端队列
        std::list<int> lst = {1000, 2000, 3000};         // 双向链表
        std::forward_list<int> flst = {5, 4, 3, 2, 1};   // 单向链表

        // 2. 关联容器
        std::set<int> s = {3, 1, 4, 1, 5};               // 有序集合（去重）
        std::multiset<int> ms = {3, 1, 4, 1, 5};         // 有序多重集合（不去重）
        std::unordered_set<int> us = {2, 7, 1, 8};       // 无序集合
        std::map<int, std::string> mp = {{1, "one"}, {2, "two"}}; // 有序映射
        std::unordered_map<int, std::string> ump = {{3, "three"}, {4, "four"}}; // 无序映射

        debug(vec, arr, deq, lst, flst, s, ms, us); // 8参数调用
        debug(mp, ump); // 单独测试map
    }

    // 测试3：配对与元组类型（pair、tuple）
    void testPairAndTuple() {
        std::cerr << "\n===== 测试3：配对与元组类型 =====" << std::endl;
        // 1. pair（键值对）
        std::pair<int, std::string> p1 = {1, "apple"};
        std::pair<double, bool> p2 = {3.14, false};

        // 2. tuple（多元组，最多支持8个元素）
        std::tuple<int, double, char> t1 = {10, 2.5, 'X'};
        std::tuple<std::string, std::vector<int>, std::set<int>> t2 = {"test", {1,2,3}, {4,5,6}}; // 嵌套容器

        debug(p1, p2, t1, t2);
    }

    // 测试4：自定义聚合类型（结构体）C++17
    void testCustomAggregate() {
        std::cerr << "\n===== 测试4：自定义聚合类型（结构体） =====" << std::endl;
        Point2D pt = {5, 10};                          // 2字段结构体
        Student stu = {"Bob", 20, 88.9f};              // 3字段结构体
        Rectangle rect = {0, 0, 100, 200};             // 4字段结构体
        Line2D line = {pt, Point2D{3, 4}};
        // 嵌套结构体
        std::vector<Point2D> pt_list = {{1,2}, {3,4}, {5,6}};

        debug(pt, stu, rect, pt_list, line);
    }

    // 测试5：C风格数组与字符串
    void testCArrayAndString() {
        std::cerr << "\n===== 测试5：C风格数组与字符串 =====" << std::endl;
        int c_arr1[5] = {1, 3, 5, 7, 9};  // C风格整型数组
        char c_arr2[6] = "hello";         // C风格字符数组（字符串）
        const char* c_str = "world";      // C风格字符串指针

        debug(c_arr1, c_arr2, c_str);
    }

private:
    // 私有构造函数（禁止外部实例化）
    DebugTester() {
        std::cerr << "===== 竞赛调试工具单例测试启动 =====" << std::endl;
    }

    // 私有析构函数
    ~DebugTester() {
        std::cerr << "\n===== 所有测试完成 =====" << std::endl;
    }
};

// 主函数：调用单例测试所有功能
int main() {
    // 获取单例实例
    DebugTester& tester = DebugTester::getInstance();

    // 依次执行所有测试
    tester.testBasicTypes();
    tester.testSTLContainers();
    tester.testPairAndTuple();
    tester.testCustomAggregate(); // C++ 17
    tester.testCArrayAndString();

    return 0;
}



*/
```

### FastInputOutput
<!-- @code FastInputOutput -->

# last. CTL

克隆本仓库后使用 ctl 维护模板。安装 snippets 执行 `ctl update --local`，更新语雀执行 `ctl update --yuque`；其余命令见 `ctl --help`。

<!-- @tool CTL -->
