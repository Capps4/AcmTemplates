#pragma once

// Clang may also use libstdc++; select by availability, not compiler name.
#if __has_include(<bits/stdc++.h>)
// The local bits header appends a debugger with non-inline definitions.
// Standard headers must not activate it in every translation unit.
#ifdef COMPETITION_DEBUGER
#include <bits/stdc++.h>
#else
#define COMPETITION_DEBUGER
#include <bits/stdc++.h>
#undef COMPETITION_DEBUGER
#endif
#else
#include "Std.hpp"
#endif

// Keep assertions available on both standard-library implementations.
#include <cassert>
