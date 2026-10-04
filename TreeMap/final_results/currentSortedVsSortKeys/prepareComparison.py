from pathlib import Path
import hashlib
import json
import subprocess
import tempfile

report = Path(__file__).resolve().parent
root = report.parents[2]
build = Path(tempfile.mkdtemp(prefix='tree-map-current-sorted-'))
source = (report / 'CurrentTreeMap.source').read_text()
snippet = json.loads((report / 'CurrentListHelper.code-snippets').read_text())
body = snippet['Print to console']['body']
includes = (root / 'list_helper_cpp17/ListHelperAdaptive.hpp').read_text().split('namespace seq {', 1)[0]
helper = includes + '\n'.join(line.replace('\\\\', '\\') for line in body if line != '$0') + '\n'
(build / 'CurrentListHelper.hpp').write_text(helper)
(build / 'treeMap.hpp').write_text(source)
subprocess.run(['patch', '-s', '-p1', '-d', str(build), '-i',
                str(report / 'RestoreInternalSort.patch')], check=True)
legacy = (build / 'treeMap.hpp').read_text()
bridge = source.replace('#include "../list_helper_cpp17/ListHelperAdaptive.hpp"',
                        '#include "CurrentListHelper.hpp"')
start = bridge.index('        if constexpr (std::is_integral_v<Key> &&', bridge.index('explicit TreeMapOff('))
end = bridge.index('        keys.erase(', start)
direct = bridge[:start] + '        keys = std::move(keys) | seq::sorted(compare);\n' + bridge[end:]
for namespace, name, content in [('legacyMap', 'Legacy.hpp', legacy),
                                  ('bridgeMap', 'Bridge.hpp', bridge),
                                  ('directMap', 'Direct.hpp', direct)]:
    content = content.replace('_treemap', namespace)
    content = content.replace('using ' + namespace + '::TreeMap;', '')
    content = content.replace('using ' + namespace + '::TreeMapOff;', '')
    (build / name).write_text(content)
check = (root / 'tree_map_experiments/check.cpp').read_text()
check = check.replace('#include "treeMap.hpp"', '#include "Direct.hpp"\nusing directMap::TreeMap;\nusing directMap::TreeMapOff;')
(build / 'integration.cpp').write_text(check)
hashes = {name: hashlib.sha256((build / name).read_bytes()).hexdigest()
          for name in ['CurrentListHelper.hpp', 'Legacy.hpp', 'Bridge.hpp', 'Direct.hpp']}
hashes.update({str(path.relative_to(root)): hashlib.sha256(path.read_bytes()).hexdigest()
               for path in [report / 'CurrentListHelper.code-snippets', report / 'CurrentTreeMap.source',
                            report / 'RestoreInternalSort.patch', report / 'benchmark.cpp']})
(report / 'sourceSha256.json').write_text(json.dumps(hashes, indent=2) + '\n')
print(build)
