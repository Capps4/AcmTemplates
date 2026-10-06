// Debugger first: generic traits need no container declarations or definitions.
#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"
#include "../../../../src/DataStructures/BaseDataStructures/HashMap/code.hpp"
#include "../../../../src/DataStructures/TreeDataStructures/TreeMap/code.hpp"
#include "../../../../src/Clarketech/DebugerHeadCopyToBitsStdC++/code.hpp"

static_assert(std::is_same_v<_hashmap::Impl<int, 17, 8>::key_type, u64>);
static_assert(std::is_same_v<_hashmap::Impl<int, 17, 8>::mapped_type, int>);
static_assert(std::is_same_v<TreeMap<int, bool>::key_type, int>);
static_assert(std::is_same_v<TreeMap<int, bool>::mapped_type, bool>);
static_assert(std::is_same_v<TreeMapOff<int, bool>::key_type, int>);
static_assert(std::is_same_v<TreeMapOff<int, bool>::mapped_type, bool>);
static_assert(debuger::Map<_hashmap::Impl<int, 17, 8>>::value);
static_assert(debuger::Map<TreeMap<int, int>>::value);
static_assert(debuger::Map<TreeMapOff<int, int>>::value);
static_assert(debuger::Map<std::map<int, int>>::value);
static_assert(not debuger::Map<std::set<int>>::value);
static_assert(not debuger::Map<std::vector<std::pair<int, int>>>::value);

debuger::Cfg *otherConfig() { return &debuger::cfg(); }
bool *otherBusy() { return &debuger::busy(); }
void otherLog(double value) { debuger::log(9, "value", value); }
std::string otherMaps() {
    _hashmap::Impl<int, 17, 8> hash;
    TreeMap<int, int> tree;
    TreeMapOff<int, int> offline({4});
    hash[2] = 7; tree[3] = 9; offline[4] = 11;
    std::ostringstream out;
    debuger::put(out, std::as_const(hash)); out << '|';
    debuger::put(out, std::as_const(tree)); out << '|';
    debuger::put(out, std::as_const(offline));
    return out.str();
}
