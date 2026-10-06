#include "../../../../../src/String/StringF4/StringHash/code.hpp"
_strhash::u64 otherHash(std::string_view text) { return StringHash(text).getU64(0); }
const int* otherHashBases() { return _strhash::b; }
