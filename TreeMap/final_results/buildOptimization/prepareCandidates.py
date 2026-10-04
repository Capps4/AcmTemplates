from pathlib import Path
import hashlib
import shutil
import subprocess
import tempfile

report = Path(__file__).resolve().parent
root = report.parent.parent
output = Path(tempfile.mkdtemp(prefix="treeMapBuildRepro-"))
expected = dict(line.split()[::-1] for line in (report / "sourceSha256.txt").read_text().splitlines())
header = root / "treeMap.hpp"
if hashlib.sha256(header.read_bytes()).hexdigest() != expected["treeMap.hpp"]:
    raise SystemExit("Current header differs from the measured version; restore that version first.")
before = output / "before.hpp"
shutil.copyfile(header, before)
subprocess.run(["patch", "-s", "-R", str(before)],
               input=(report / "optimization.patch").read_text(), text=True, check=True)
assert hashlib.sha256(before.read_bytes()).hexdigest() == expected["before.hpp"]
shutil.copyfile(before, output / "base.hpp")
for name, change in [("outlined", "sbtOutlined"), ("result", "sbtResult"),
                     ("combined", "sbtCombined"), ("compact", "offCompact"),
                     ("index32", "offIndex32"), ("radix", "offRadix")]:
    target = output / (name + ".hpp")
    shutil.copyfile(before, target)
    subprocess.run(["patch", "-s", str(target)],
                   input=(report / (change + ".patch")).read_text(), text=True, check=True)
print(output)
