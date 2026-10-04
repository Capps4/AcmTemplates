#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
buildDir=$(python3 prepareComparison.py)
flags=(-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror)
g++-15 -std=gnu++17 "${flags[@]}" -I "$buildDir" benchmark.cpp -o "$buildDir/gcc"
clang++ -std=c++17 "${flags[@]}" -I "$buildDir" benchmark.cpp -o "$buildDir/clang"
clang++ -std=c++17 -O1 -g -Wall -Wextra -Wpedantic -Werror \
    -fsanitize=address,undefined -fno-omit-frame-pointer "$buildDir/integration.cpp" -o "$buildDir/check"
"$buildDir/check" > validation.txt
"$buildDir/gcc" > gcc.csv 2> gcc.log
"$buildDir/clang" > clang.csv 2> clang.log
python3 analyze.py > metrics.txt
