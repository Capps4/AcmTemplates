"""Complete-document publication used only by ctl update --yuque."""
import difflib
import hashlib
import json
from pathlib import Path

import tools as config
from tools.Common import write_json
from tools.Library import build
from tools.Yuque import STATE, YuqueError, body_of, parse_ref, ensure_client


def state_path(ref):
    return STATE / 'publish' / (hashlib.sha256(ref.encode()).hexdigest()[:20] + '.json')


def normalize(text):
    return text.replace('\r\n', '\n').rstrip('\n')


def prepare(client, result):
    book, slug = parse_ref(config.YUQUE_DOCUMENT)
    ref = book + '/' + slug
    current = client.get(ref)
    old, new = normalize(body_of(current)), normalize(result['lake'])
    folder = config.GENERATED
    folder.mkdir(parents=True, exist_ok=True)
    lake = folder / 'Library.lake'
    lake.write_text(result['lake'], encoding='utf-8')
    (folder / 'Library.md').write_text(result['markdown'], encoding='utf-8')
    write_json(folder / 'Sources.json', result['provenance'])
    old_markdown = body_of(current, 'markdown')
    diff = ''.join(difflib.unified_diff(old_markdown.splitlines(keepends=True),
                                      result['markdown'].splitlines(keepends=True),
                                      fromfile='yuque/Library.md', tofile='git/Library.md'))
    diff_path = folder / 'Yuque.diff'
    diff_path.write_text(diff, encoding='utf-8')
    saved = state_path(ref)
    previous = json.loads(saved.read_text(encoding='utf-8')) if saved.exists() else None
    baseline = (config.YUQUE_EXPECTED_REVISION or
                (previous['revision'] if previous else config.INITIAL_REVISIONS.get(ref)))
    conflict = bool(baseline and baseline != current['_yuque']['revision'])
    write_json(STATE / 'publish/base.json', current)
    return {'ref': ref, 'changed': old != new, 'remoteChanged': conflict,
            'revision': current['_yuque']['revision'], 'prepared': str(lake), 'diff': str(diff_path)}, current


def publish(console, client=None):
    with console.step('组装正文和模板代码'):
        result = build()
        console.note(str(result['provenance']['templates']) + ' 个模板；保留标题锚点和折叠代码卡片')
    with console.step('检查语雀配置与认证'):
        client = client or ensure_client(console)
    with console.step('读取远端并检查差异'):
        report, current = prepare(client, result)
        console.note('目标：' + report['ref'])
        console.note('差异：' + report['diff'])
        if report['remoteChanged']:
            raise YuqueError('远端已被修改，未覆盖。先把需要的变更合入 Library.md，检查差异文件；'
                             '确认后将 tools/__init__.py 的 YUQUE_EXPECTED_REVISION 设置为：\n' + report['revision'], 6)
    saved = state_path(report['ref'])
    if not report['changed']:
        write_json(saved, {'ref': report['ref'], 'revision': current['_yuque']['revision']})
        return dict(report, applied=False, verified=True)
    with console.step('检查现版正确性与性能报告'):
        from tools.Verification import save
        evidence, path = save()
        console.note('报告：' + str(path))
        if not evidence['passed']:
            raise YuqueError('全库正确性或性能验证缺失、失败或过期。请先运行 ctl test --all；发布未执行。', 6)
        if build()['provenance'] != result['provenance'] or Path(report['prepared']).read_text(encoding='utf-8') != result['lake']:
            raise YuqueError('生成后本地文档或代码已变化，请重新执行 ctl update --yuque。', 6)
    with console.step('备份、发布并回读验证'):
        after = client.update(report['ref'], report['prepared'], current)
        write_json(STATE / 'publish/after.json', after)
        if not after['_yuque'].get('verified'):
            raise YuqueError('写入已执行，但回读 Lake 不一致；请检查 .yuque/publish/after.json 和备份，勿直接重试。', 6)
        write_json(saved, {'ref': report['ref'], 'revision': after['_yuque']['revision'],
                          'provenance': result['provenance']})
        console.note('备份：' + after['_yuque']['backup'])
    return dict(report, applied=True, verified=True, backup=after['_yuque']['backup'])
