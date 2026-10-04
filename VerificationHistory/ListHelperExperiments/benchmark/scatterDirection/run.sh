#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
python3 prepare.py
buildDir=$(mktemp -d "${TMPDIR:-/private/tmp}/scatter-direction.XXXXXX")
flags=(-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror)
g++-15 -std=gnu++17 "${flags[@]}" benchmark.cpp -o "$buildDir/gcc"
clang++ -std=c++17 "${flags[@]}" benchmark.cpp -o "$buildDir/clang"
g++-15 --version > compiler-gcc.txt
clang++ --version > compiler-clang.txt
seeds=(177613 829319 555109)
for launch in 0 1 2; do
    "$buildDir/gcc" "${seeds[$launch]}" 7 > "gcc-$launch.csv" 2> "gcc-$launch.log"
    "$buildDir/clang" "${seeds[$launch]}" 7 > "clang-$launch.csv" 2> "clang-$launch.log"
done
python3 analyze.py > metrics.txt
