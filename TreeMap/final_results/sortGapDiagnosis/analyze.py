import csv,json,statistics
from collections import defaultdict
from pathlib import Path
root=Path(__file__).resolve().parent
names=['legacy','current','swap','forward','forwardSwap','heapBuckets','invertedDigits','combined']
reports={}
for compiler in ['gcc','clang']:
    groups=defaultdict(list)
    for row in csv.DictReader((root/(compiler+'.csv')).open()):
        key=tuple(row[k] for k in ['type','distribution','direction'])
        groups[key,int(row['method'])].append(float(row['ns']))
    output=[]
    print(compiler)
    for key in sorted({key for key,method in groups}):
        times={name:statistics.median(groups[key,i])/1e6 for i,name in enumerate(names)}
        output.append(dict(zip(['type','distribution','direction'],key),timesMs=times))
        print(key, {k:round(v,4) for k,v in times.items()})
    reports[compiler]=output
(root/'metrics.json').write_text(json.dumps(reports,indent=2)+'\n')
