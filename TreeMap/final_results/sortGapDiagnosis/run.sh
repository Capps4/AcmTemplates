#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
python3 prepare.py
buildDir=$(mktemp -d "${TMPDIR:-/private/tmp}/sort-gap-run.XXXXXX")
flags=(-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror)
g++-15 -std=gnu++17 "${flags[@]}" benchmark.cpp -o "$buildDir/gcc"
clang++ -std=c++17 "${flags[@]}" benchmark.cpp -o "$buildDir/clang"
g++-15 -std=gnu++17 "${flags[@]}" histogram.cpp -o "$buildDir/histogramGcc"
clang++ -std=c++17 "${flags[@]}" histogram.cpp -o "$buildDir/histogramClang"
"$buildDir/gcc" > gcc.csv 2> gcc.log
"$buildDir/clang" > clang.csv 2> clang.log
"$buildDir/histogramGcc" > histogram-gcc.csv
"$buildDir/histogramClang" > histogram-clang.csv
python3 analyze.py > metrics.txt
