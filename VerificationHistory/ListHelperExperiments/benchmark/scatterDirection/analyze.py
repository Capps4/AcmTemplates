from pathlib import Path
from collections import defaultdict
import csv
import json
import math
import statistics

root = Path(__file__).resolve().parent
fields = ['stage', 'container', 'type', 'n', 'distribution', 'direction', 'bits']
summary = []
for compiler in ['gcc', 'clang']:
    groups = defaultdict(list)
    for launch in range(3):
        with (root / f'{compiler}-{launch}.csv').open() as f:
            for row in csv.DictReader(f):
                key = tuple(row[k] for k in fields)
                groups[key, launch, int(row['method'])].append(float(row['ns']))
    for key in sorted({k for k, launch, method in groups}):
        back = [statistics.median(groups[key, launch, 0]) for launch in range(3)]
        forward = [statistics.median(groups[key, launch, 1]) for launch in range(3)]
        ratios = [f / b for f, b in zip(forward, back)]
        item = dict(zip(fields, key), compiler=compiler,
                    backwardMs=statistics.median(back) / 1e6,
                    forwardMs=statistics.median(forward) / 1e6,
                    ratio=math.exp(statistics.mean(map(math.log, ratios))),
                    launchRatios=ratios)
        summary.append(item)
(root / 'metrics.json').write_text(json.dumps(summary, indent=2) + '\n')
with (root / 'summary.csv').open('w') as f:
    writer = csv.DictWriter(f, fieldnames=list(summary[0]))
    writer.writeheader()
    writer.writerows(summary)
def print_group(label, rows):
    ratios = [r['ratio'] for r in rows]
    geo = math.exp(statistics.mean(map(math.log, ratios)))
    consistent_faster = sum(all(x < .98 for x in r['launchRatios']) for r in rows)
    consistent_slower = sum(all(x > 1.02 for x in r['launchRatios']) for r in rows)
    print(label, 'cases=', len(rows), 'geoRatio=', round(geo, 4),
          'consistentFaster2%=', consistent_faster, 'consistentSlower2%=', consistent_slower)
for compiler in ['gcc', 'clang']:
    print('\n' + compiler)
    rows = [r for r in summary if r['compiler'] == compiler]
    # Exclude no-radix control cases from the primary aggregation.
    for container in ['vector', 'array']:
        active = [r for r in rows if r['stage'] == 'sorted' and r['container'] == container
                  and int(r['n']) >= 4096 and r['distribution'] in ['full', 'bits22', 'gaps', 'few', 'nearly']]
        print_group('active sorted ' + container, active)
    large = [r for r in rows if r['stage'] == 'sorted' and r['container'] == 'vector' and r['n'] == '1000000']
    for r in large:
        print('million', r['type'], r['distribution'], r['direction'],
              round(r['backwardMs'], 4), round(r['forwardMs'], 4),
              round(r['ratio'], 4), [round(x, 3) for x in r['launchRatios']])
    for bits in ['8', '11', '15']:
        print_group('kernel B=' + bits, [r for r in rows if r['stage'] == 'kernel' and r['bits'] == bits])
    stable_losses = [r for r in rows if r['stage'] == 'sorted' and r['distribution'] in ['full', 'bits22', 'gaps', 'few', 'nearly']
                     and int(r['n']) >= 4096 and all(x > 1.02 for x in r['launchRatios'])]
    for r in sorted(stable_losses, key=lambda r: r['ratio'], reverse=True)[:6]:
        print('consistent loss', r)
