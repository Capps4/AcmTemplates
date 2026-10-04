#!/usr/bin/env python3
from pathlib import Path
import hashlib
import shutil
import subprocess
import tempfile

report = Path(__file__).resolve().parent
root = report.parents[1]
expected = dict(line.split(None, 1)[::-1] for line in (report / "sourceSha256.txt").read_text().splitlines())
for name in ["treeMap.hpp", "sort.hpp", "benchmark.cpp"]:
    if hashlib.sha256((root / name).read_bytes()).hexdigest() != expected[name]:
        raise SystemExit("Source changed since this experiment: " + name)
directory = Path(tempfile.mkdtemp(prefix="treeMap-comparison-"))
previous = directory / "previous"
previous.mkdir()
shutil.copyfile(root / "treeMap.hpp", previous / "treeMap.hpp")
subprocess.run(["patch", "-s", "-p1", "-d", str(previous), "-i", str(report / "restorePreviousTreeMap.patch")], check=True)
old = (previous / "treeMap.hpp").read_bytes()
if hashlib.sha256(old).hexdigest() != expected["historicalTreeMap.hpp"]:
    raise SystemExit("Historical header checksum mismatch")
sortInclude = '#include "' + str(root / "sort.hpp") + '"'
header = (root / "treeMap.hpp").read_text().replace('#include "sort.hpp"', sortInclude)
(directory / "treeMap.hpp").write_text(header)
header = old.decode().replace("_treemap", "_treemapBefore").replace('#include "sort.hpp"', sortInclude)
header = header.replace("using _treemapBefore::TreeMap;", "").replace("using _treemapBefore::TreeMapOff;", "")
(directory / "previousTreeMap.hpp").write_text(header)
core = (root / "benchmark.cpp").read_text()
head, tail = core.rsplit("\n}", 1)
core = head + "\n    return 0;\n}" + tail
# Bounds check is outside timing; it avoids a GCC warning in the specialized harness.
core = core.replace("expected.flows[static_cast<int>(work)]", "expected.flows.at(static_cast<std::size_t>(work))")
(directory / "benchmarkCore.cpp").write_text(core)
shutil.copyfile(report / "compare.cpp", directory / "compare.cpp")
print(directory)
