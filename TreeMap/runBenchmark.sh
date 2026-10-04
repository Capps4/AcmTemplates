#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

GCC_CXX="${GCC_CXX:-g++-15}"
CLANG_CXX="${CLANG_CXX:-clang++}"
BENCH_N="${BENCH_N:-1000000}"
BENCH_Q="${BENCH_Q:-1000000}"
BENCH_ROUNDS="${BENCH_ROUNDS:-5}"
BENCH_SEED="${BENCH_SEED:-20260929}"
for number in "$BENCH_N" "$BENCH_Q" "$BENCH_ROUNDS"; do
    if [[ ! "$number" =~ ^[1-9][0-9]*$ ]]; then
        echo "BENCH_N, BENCH_Q and BENCH_ROUNDS must be positive integers" >&2
        exit 2
    fi
done
if [[ ! "$BENCH_SEED" =~ ^[0-9]+$ ]]; then
    echo "BENCH_SEED must be a nonnegative integer" >&2
    exit 2
fi
command -v "$GCC_CXX" >/dev/null
command -v "$CLANG_CXX" >/dev/null
mkdir -p build final_results

runAll() {
    local gnuFlags=(-std=gnu++17 -O2 -Wall -Wextra -Wpedantic)
    local checkFlags=(-std=c++17 -O2 -Wall -Wextra -Wpedantic)
    local benchmarkFlags=(-std=gnu++17 -O2 -DNDEBUG -Wall -Wextra -Wpedantic)
    local sanitizerFlags=(-std=c++17 -O1 -g -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer)
    {
        date -u '+UTC %Y-%m-%d %H:%M:%S'
        uname -a
        sysctl -n machdep.cpu.brand_string 2>/dev/null || true
        "$GCC_CXX" --version
        "$CLANG_CXX" --version
        echo "GCC GNU++17 check flags: ${gnuFlags[*]}"
        echo "GCC/Clang strict C++17 check flags: ${checkFlags[*]}"
        echo "Sanitizer flags: ${sanitizerFlags[*]}"
        echo "Benchmark flags: ${benchmarkFlags[*]}"
        echo "N=$BENCH_N Q=$BENCH_Q rounds=$BENCH_ROUNDS seed=$BENCH_SEED"
        echo "Random int keys; TreeMap/TreeMapOff/std::map values are bool; std::set/PBDS are sets."
        echo "Online TreeMap uses fixed N-slot construction; TreeMapOff receives 2N candidate keys."
        echo "Build includes allocation and offline candidate copy/sort. All other setup and all final checks are outside timing."
        echo "One verified warmup per container/workload; measured container order is shuffled each round."
        echo "Independent segment-tree order-statistics oracle and complete final key/value verification."
        echo "Result-stream hashing is inside timing; iteration hashes keys consistently for all five containers."
        echo "std::set/std::map rankOf, keyAt and orderMixed are N/A."
    } > final_results/environment.txt

    echo "Compiling GNU++17, strict C++17 and sanitizer correctness checks"
    "$GCC_CXX" "${gnuFlags[@]}" check.cpp -o build/checkGcc
    "$GCC_CXX" "${checkFlags[@]}" check.cpp -o build/checkGccStrict
    "$CLANG_CXX" "${checkFlags[@]}" check.cpp -o build/checkClang
    "$CLANG_CXX" "${sanitizerFlags[@]}" check.cpp -o build/checkSanitized
    echo "Running GCC GNU++17 correctness checks"
    ./build/checkGcc | tee final_results/checkGcc.txt
    echo "Running GCC strict C++17 correctness checks"
    ./build/checkGccStrict | tee final_results/checkGccStrict.txt
    echo "Running Clang correctness checks"
    ./build/checkClang | tee final_results/checkClang.txt
    echo "Running ASan/UBSan correctness checks"
    ./build/checkSanitized | tee final_results/checkSanitized.txt

    echo "Compiling GCC benchmark after all correctness checks passed"
    "$GCC_CXX" "${benchmarkFlags[@]}" benchmark.cpp -o build/benchmark
    echo "Starting serial five-container benchmark"
    ./build/benchmark --n "$BENCH_N" --q "$BENCH_Q" --rounds "$BENCH_ROUNDS" \
        --seed "$BENCH_SEED" --summary final_results/summary.csv > final_results/raw.csv
    shasum -a 256 Final.hpp ../ListHelper/Final.hpp check.cpp benchmark.cpp runBenchmark.sh > final_results/sourceSha256.txt
    echo "Completed: final_results/raw.csv, final_results/summary.csv and final_results/run.log"
}
# An ordinary pipeline avoids /dev/fd process substitution in restricted shells.
runAll 2>&1 | tee final_results/run.log
