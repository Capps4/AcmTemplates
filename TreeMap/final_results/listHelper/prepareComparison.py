#!/usr/bin/env python3
from pathlib import Path
import hashlib
import json
import shutil
import subprocess
import tempfile

report = Path(__file__).resolve().parent
root = report.parents[2]
module = root / "tree_map_experiments"
expected = json.loads((report / "sourceSha256.json").read_text())
for name in ["tree_map_experiments/treeMap.hpp", "list_helper_cpp17/ListHelperAdaptive.hpp",
             "tree_map_experiments/final_results/listHelper/compare.cpp",
             "tree_map_experiments/final_results/listHelper/restoreInternalSort.patch"]:
    if hashlib.sha256((root / name).read_bytes()).hexdigest() != expected[name]:
        raise SystemExit("Source changed since this experiment: " + name)

directory = Path(tempfile.mkdtemp(prefix="treeMap-listHelper-comparison-"))
shutil.copyfile(module / "treeMap.hpp", directory / "treeMap.hpp")
subprocess.run(["patch", "-s", "-p1", "-d", str(directory), "-i",
                str(report / "restoreInternalSort.patch")], check=True)
baseline = directory / "treeMap.hpp"
if hashlib.sha256(baseline.read_bytes()).hexdigest() != expected["baselineTreeMap.hpp"]:
    raise SystemExit("Baseline checksum mismatch")
baseline.rename(directory / "baseline.hpp")

source = (module / "treeMap.hpp").read_text()
source = source.replace('#include "../list_helper_cpp17/ListHelperAdaptive.hpp"',
                        '#include "' + str(root / "list_helper_cpp17/ListHelperAdaptive.hpp") + '"')
start = source.index("        if constexpr (std::is_integral_v<Key> &&", source.index("explicit TreeMapOff("))
end = source.index("        keys.erase(", start)
direct = source[:start] + "        keys = std::move(keys) | seq::sorted(compare);\n" + source[end:]
for namespace, header in [("candidate", source), ("direct", direct)]:
    header = header.replace("_treemap", namespace)
    header = header.replace("using " + namespace + "::TreeMap;", "")
    header = header.replace("using " + namespace + "::TreeMapOff;", "")
    (directory / (namespace + ".hpp")).write_text(header)
shutil.copyfile(report / "compare.cpp", directory / "compare.cpp")
print(directory)
