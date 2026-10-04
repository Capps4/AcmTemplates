#include "Final.hpp"

#include <iostream>
#include <optional>
#include <string>

struct StringInfo { int pass = 0, end = 0; };
using Tree = StringTrie<StringInfo>;

// 在 walk 的根回调中自定义 DFS；Node[d] 只读访问孩子。
std::optional<std::string> lowerBound(const Tree& tree, std::string_view x) {
    std::string res;
    bool found = false;
    auto dfs = [&](auto&& dfs, const Tree::Node& u, int dep, bool tight) -> bool {
        if (u.info.pass == 0) return false;
        if ((!tight || dep == int(x.size())) && u.info.end > 0) return true;
        const int start = tight && dep < int(x.size()) ? x[dep] - 'a' : 0;
        for (int d = start; d < Tree::degree; ++d) {
            res.push_back(char('a' + d));
            if (dfs(dfs, u[d], dep + 1, tight && dep < int(x.size()) && d == start)) return true;
            res.pop_back();
        }
        return false;
    };
    tree.walk([&](const Tree::Node& u, int) { found = dfs(dfs, u, 0, true); return -1; });
    if (found) return res;
    return std::nullopt;
}

int main() {
    Tree::clearInit();
    Tree tree;
    for (const std::string_view s : {"app", "apple", "bat", "cat"}) {
        tree.modify(s, [&](StringInfo& info, int dep) {
            ++info.pass;
            if (dep == int(s.size())) ++info.end;
        });
    }
    for (const auto x : {"ap", "app", "az", "dog"}) {
        const auto ans = lowerBound(tree, x);
        std::cout << x << " -> " << (ans ? *ans : "none") << '\n';
    }
    return 0;
}
