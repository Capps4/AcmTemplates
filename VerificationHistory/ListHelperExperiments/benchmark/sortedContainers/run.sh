#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
buildDir=$(mktemp -d "${TMPDIR:-/private/tmp}/sorted-containers.XXXXXX")
flags=(-O2 -Wall -Wextra -Wpedantic -Werror)
g++-15 -std=gnu++17 "${flags[@]}" correctness.cpp -o "$buildDir/checkGcc"
clang++ -std=c++17 -O1 -g -Wall -Wextra -Wpedantic -Werror \
    -fsanitize=address,undefined -fno-omit-frame-pointer correctness.cpp -o "$buildDir/checkSanitized"
g++-15 -std=gnu++17 "${flags[@]}" -DNDEBUG benchmark.cpp -o "$buildDir/benchmarkGcc"
clang++ -std=c++17 "${flags[@]}" -DNDEBUG benchmark.cpp -o "$buildDir/benchmarkClang"
"$buildDir/checkGcc" > check-gcc.txt
"$buildDir/checkSanitized" > check-sanitized.txt
"$buildDir/benchmarkGcc" > gcc.csv
"$buildDir/benchmarkClang" > clang.csv
python3 analyze.py > metrics.txt
