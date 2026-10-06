#include "../../../../../src/DataStructures/TreeDataStructures/TreeMap/code.hpp"
#include <cassert>
#include <vector>

int main() {
    TreeMap<int, int> map;
    map[5] = 7;
    map[2] += 3;
    assert(!map(9) && map.size() == 2); // 缺失读取返回 nullopt，不插入
    if (auto value = map(5); value)
        assert(*value == 7); // optional 保存独立副本
    assert(map.rankOf(5) == 1);
    assert(map.keyAt(0) == 2);
    for (auto [key, value] : map) value += key;

    TreeMapOff<int, bool> marks(std::vector<int>{2, 5, 9, 5});
    marks[5] = true;
    marks[5] = false;
    assert(marks.contains(5)); // false 也是已存在的值
    marks.erase(5);
    return 0;
}
