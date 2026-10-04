#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
benchmarkFlags=(-O2 -DNDEBUG -Wall -Wextra -Wpedantic -Werror)
checkFlags=(-O2 -Wall -Wextra -Wpedantic -Werror)
sanitizerFlags=(-O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer)
buildDir=$(mktemp -d "${TMPDIR:-/private/tmp}/sorted-optimization.XXXXXX")
g++-15 -std=gnu++17 "${checkFlags[@]}" correctness.cpp -o "$buildDir/checkGcc"
g++-15 -std=c++17 "${checkFlags[@]}" correctness.cpp -o "$buildDir/checkGccStrict"
clang++ -std=c++17 "${sanitizerFlags[@]}" correctness.cpp -o "$buildDir/checkSanitized"
"$buildDir/checkGcc" > final-check-gcc.txt
"$buildDir/checkGccStrict" > final-check-gcc-strict.txt
"$buildDir/checkSanitized" > final-check-sanitized.txt
g++-15 -std=gnu++17 "${benchmarkFlags[@]}" benchmark.cpp -o "$buildDir/benchmarkGcc"
clang++ -std=c++17 "${benchmarkFlags[@]}" benchmark.cpp -o "$buildDir/benchmarkClang"
for phase in 1 2 3 4; do
    if [[ "$phase" == 4 ]]; then
        name=holdout
    else
        name=stage$phase
    fi
    "$buildDir/benchmarkGcc" "$phase" > "$name-gcc.csv"
    "$buildDir/benchmarkClang" "$phase" > "$name-clang.csv"
done
python3 analyze.py > metrics.log
