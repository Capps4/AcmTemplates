from pathlib import Path
from collections import defaultdict
import csv
import json
import math
import statistics

root = Path(__file__).resolve().parent
dimensions = ["type", "n", "bits", "gaps", "pattern", "comparator"]
summary = {}
rows = []
for phase in ["stage1", "stage2", "stage3", "holdout"]:
    for compiler in ["gcc", "clang"]:
        groups = defaultdict(list)
        for item in csv.DictReader((root / (phase + "-" + compiler + ".csv")).open()):
            key = tuple(item[name] for name in dimensions) + (item["method"],)
            groups[key].append(float(item["ns"]))
        for key, values in groups.items():
            if len(values) != 5:
                raise RuntimeError("Expected five rounds: " + str(key))
            median = statistics.median(values)
            summary[(phase, compiler, *key)] = median
            rows.append([phase, compiler, *key, len(values), median, min(values), max(values)])

with (root / "summary.csv").open("w") as output:
    writer = csv.writer(output)
    writer.writerow(["phase", "compiler", *dimensions, "method", "samples", "medianNs", "minNs", "maxNs"])
    writer.writerows(rows)

metrics = {}
for compiler in ["gcc", "clang"]:
    ratios = defaultdict(list)
    worst = []
    for key, old in summary.items():
        phase, cc, kind, n, bits, gaps, pattern, comparator, method = key
        if phase != "holdout" or cc != compiler or method != "0":
            continue
        new = summary[key[:-1] + ("6",)]
        ratio = new / old
        group = "randomDefault" if pattern == "0" and gaps == "0" and comparator == "lessVoid" else "other"
        ratios[group].append(ratio)
        if group == "randomDefault" and int(n) >= 4096:
            ratios["randomDefaultLarge"].append(ratio)
        if pattern == "0" and comparator in ["lessTyped", "greaterTyped"]:
            ratios["randomTyped"].append(ratio)
        if pattern == "0" and gaps != "0":
            ratios["gaps"].append(ratio)
        if pattern in ["1", "2"]:
            ratios["ordered"].append(ratio)
        worst.append((ratio, key))
    metrics[compiler] = {
        group: {"cases": len(values), "timeRatioGeomean": math.exp(sum(map(math.log, values)) / len(values))}
        for group, values in ratios.items()
    }
    metrics[compiler]["worstDefault"] = sorted(
        [(ratio, key) for ratio, key in worst if key[-2] == "lessVoid"], reverse=True
    )[:5]
(root / "metrics.json").write_text(json.dumps(metrics, indent=2) + "\n")
print(json.dumps(metrics, indent=2))
