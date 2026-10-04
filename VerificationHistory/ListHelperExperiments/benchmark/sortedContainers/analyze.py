import csv, json, math, statistics
from collections import defaultdict
from pathlib import Path

root = Path(__file__).parent
reports = {}
for compiler in ['gcc', 'clang']:
    data = defaultdict(list)
    with (root / f'{compiler}.csv').open() as f:
        for row in csv.DictReader(f):
            key = tuple(row[k] for k in ['kind', 'container', 'type', 'n', 'distribution', 'comparator'])
            data[key, int(row['method'])].append(float(row['ns']))
    medians = {key: statistics.median(values) for key, values in data.items()}
    output = []
    for key in sorted({key for key, method in data}):
        methods = [3, 4, 5] if key[0] == 'fallback' else [0, 1, 2, 6]
        times = {method: medians[key, method] for method in methods}
        output.append(dict(zip(['kind', 'container', 'type', 'n', 'distribution', 'comparator'], key),
                           timesNs=times, ratio=times[4 if key[0] == "fallback" else 6]/times[methods[0]],
                           cmpRatio=times[5]/times[3] if key[0] == "fallback" else None))
    reports[compiler] = output
    print(compiler)
    for size in ['128', '512', '4096', '1000000']:
        cases = [row['ratio'] for row in output if row['kind']=='fallback' and row['n']==size]
        geometric = math.exp(statistics.mean(math.log(x) for x in cases))
        print(' isolated fallback', size, 'ratio', round(geometric, 4), 'min/max', round(min(cases),4), round(max(cases),4))
    for size in ['128', '512', '4096', '1000000', '65536']:
        cases = [row['ratio'] for row in output if row['kind']=='pipeline' and row['n']==size]
        if cases:
            print(' pipeline', size, 'ratio', round(math.exp(statistics.mean(math.log(x) for x in cases)),4))
    for row in output:
        if (row['n']=='1000000' and row['distribution']=='full' and row['comparator']=='lessVoid') or (row['container']=='array' and row['distribution']=='full'):
            print(row)
(root / 'metrics.json').write_text(json.dumps(reports, indent=2)+'\n')
