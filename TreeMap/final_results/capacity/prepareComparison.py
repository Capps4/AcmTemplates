#!/usr/bin/env python3
from pathlib import Path
import hashlib
import shutil
import subprocess
import tempfile

report = Path(__file__).resolve().parent
root = report.parents[1]
expected = dict(line.split(None, 1)[::-1] for line in (report / "sourceSha256.txt").read_text().splitlines())
for name in ["sort.hpp", "benchmark.cpp"]:
    if hashlib.sha256((root / name).read_bytes()).hexdigest() != expected[name]:
        raise SystemExit("Source changed since this experiment: " + name)
current = (root / "treeMap.hpp").read_bytes()
checksum = hashlib.sha256(current).hexdigest()
if checksum not in [expected["treeMap.hpp"], expected["historicalTreeMap.hpp"]]:
    raise SystemExit("Source changed since this experiment: treeMap.hpp")
directory = Path(tempfile.mkdtemp(prefix="treeMap-capacity-comparison-"))
(directory / "treeMap.hpp").write_bytes(current)
reverse = checksum == expected["historicalTreeMap.hpp"]
subprocess.run(["patch", "-s", "-p1", *( ["-R"] if reverse else [] ), "-d", str(directory), "-i", str(report / "restorePreviousTreeMap.patch")], check=True)
patched = (directory / "treeMap.hpp").read_bytes()
old, fixed = (current, patched) if reverse else (patched, current)
if hashlib.sha256(old).hexdigest() != expected["historicalTreeMap.hpp"]:
    raise SystemExit("Historical header checksum mismatch")
if hashlib.sha256(fixed).hexdigest() != expected["treeMap.hpp"]:
    raise SystemExit("Fixed-capacity header checksum mismatch")
sortInclude = '#include "' + str(root / "sort.hpp") + '"'
(directory / "treeMap.hpp").write_text(old.decode().replace('#include "sort.hpp"', sortInclude))
header = fixed.decode().replace("_treemap", "_treemapFixed")
header = header.replace("using _treemapFixed::TreeMap;", "").replace("using _treemapFixed::TreeMapOff;", "")
(directory / "fixedCandidate.hpp").write_text(header.replace('#include "sort.hpp"', sortInclude))
core = (root / "benchmark.cpp").read_text()
head, tail = core.rsplit("\n}", 1)
core = head + "\n    return 0;\n}" + tail
core = core.replace("expected.flows[static_cast<int>(work)]", "expected.flows.at(static_cast<std::size_t>(work))")
(directory / "benchmarkCore.cpp").write_text(core)
for name in ["capacity.cpp", "focused.cpp"]:
    shutil.copyfile(report / name, directory / name)
print(directory)
