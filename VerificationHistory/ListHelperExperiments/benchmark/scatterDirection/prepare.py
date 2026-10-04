from pathlib import Path
import hashlib
import json

report = Path(__file__).resolve().parent
source = report.parents[1] / 'ListHelperAdaptive.hpp'
text = source.read_text()
start = text.index('template<int B, bool Descending')
end = text.index('template<class Equal = std::equal_to<>>')
prefix = text[:start]
core = text[start:end]
old = '''            for (std::size_t j = 1; j < cnt.size(); ++j)
                cnt[j] += cnt[j - 1];
            for (std::size_t j = n; j-- > 0;)
                output[--cnt[(key(first[j]) >> shift) & mask]] = first[j];'''
new = '''            std::exclusive_scan(cnt.begin(), cnt.end(), cnt.begin(), std::size_t{0});
            for (std::size_t j = 0; j < n; ++j)
                output[cnt[(key(first[j]) >> shift) & mask]++] = first[j];'''
assert core.count(old) == 1
for name, body in [('backward', core), ('forward', core.replace(old, new))]:
    header = '#include <numeric>\n' + prefix.replace('namespace seq {', 'namespace ' + name + ' {')
    (report / (name + '.hpp')).write_text(header + body + '}\n')
(report / 'sourceSha256.json').write_text(json.dumps({
    str(source): hashlib.sha256(source.read_bytes()).hexdigest()
}, indent=2) + '\n')
