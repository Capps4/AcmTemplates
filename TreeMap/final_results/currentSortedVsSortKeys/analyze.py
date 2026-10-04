import csv
import json
import math
import statistics
from collections import defaultdict
from pathlib import Path

root = Path(__file__).resolve().parent
names = ['sortKeys', 'sortedDirect', 'sortedBridge', 'stdSort']
reports = {}
summary = []
for compiler in ['gcc', 'clang']:
    groups = defaultdict(list)
    rows = list(csv.DictReader((root / f'{compiler}.csv').open()))
    for row in rows:
        key = tuple(row[k] for k in ['work', 'type', 'n', 'distribution', 'direction'])
        groups[key, int(row['method'])].append(float(row['ns']))
    cases = []
    for key in sorted({key for key, method in groups}):
        methods = range(4) if key[0] == 'sort' else range(3)
        times = {names[m]: statistics.median(groups[key, m]) for m in methods}
        case = dict(zip(['work', 'type', 'n', 'distribution', 'direction'], key), timesNs=times,
                    newRatio=times['sortedDirect']/times['sortKeys'],
                    bridgeRatio=times['sortedBridge']/times['sortedDirect'])
        cases.append(case)
        for method in methods:
            values = groups[key, method]
            summary.append(dict(compiler=compiler, **dict(zip(['work', 'type', 'n', 'distribution', 'direction'], key)),
                                method=names[method], medianMs=statistics.median(values)/1e6,
                                minMs=min(values)/1e6, maxMs=max(values)/1e6))
    reports[compiler] = cases
    print(compiler, 'recorded samples:', len(rows))
    for work in ['sort', 'ctorMove', 'ctorCopy']:
        for direction in ['asc', 'desc']:
            selected = [c for c in cases if c['work']==work and c['direction']==direction and c['n']=='1000000']
            ratios = [c['newRatio'] for c in selected]
            print(work, direction, 'cases', len(selected), 'new/old geo', math.exp(statistics.mean(map(math.log, ratios))),
                  'min/max', min(ratios), max(ratios))
    for case in cases:
        if case['n'] in ['1000000','2000000'] and case['distribution'] in ['range22','fullWidth','fewValues']:
            print(case)
(root / 'metrics.json').write_text(json.dumps(reports, indent=2)+'\n')
with (root / 'summary.csv').open('w') as f:
    writer = csv.DictWriter(f, fieldnames=list(summary[0]))
    writer.writeheader()
    writer.writerows(summary)
