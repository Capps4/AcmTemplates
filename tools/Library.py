"""Combine Library.md with repository sources and render native Yuque Lake cards."""
import copy
import hashlib
import json
from pathlib import Path
import re
from urllib.parse import quote

from tools.Catalog import ROOT, catalog, code_of

MARKER = re.compile(r'<!--\s*@(code|tool)\s+([^\s]+)\s*-->\s*\Z')
from tools import DOCUMENT, RENDER_SETTINGS as CONFIG
import tools as repository


def markdown_parser():
    try:
        from markdown_it import MarkdownIt
    except ImportError:
        raise RuntimeError('文档生成需要 markdown-it-py：python3 -m pip install -r tools/requirements.txt') from None
    md = MarkdownIt('commonmark', {'html': True}).enable('table')

    def math_rule(state, silent):
        start = state.pos
        if state.src[start] != '$':
            return False
        delimiter = '$$' if state.src.startswith('$$', start) else '$'
        end = state.src.find(delimiter, start + len(delimiter))
        while end >= 0 and state.src[end - 1] == '\\':
            end = state.src.find(delimiter, end + len(delimiter))
        if end < 0 or not state.src[start + len(delimiter):end].strip():
            return False
        if not silent:
            token = state.push('math', '', 0)
            token.content = state.src[start + len(delimiter):end].strip()
            token.meta = {'display': delimiter == '$$'}
        state.pos = end + len(delimiter)
        return True

    md.inline.ruler.before('escape', 'math', math_rule)
    return md


def settings(collection='core'):
    path = CONFIG if collection == 'core' else repository.EXTRA_RENDER_SETTINGS
    return json.loads(path.read_text(encoding='utf-8'))


def stable_id(kind, key):
    return 'acm-' + kind + '-' + hashlib.sha256(key.encode()).hexdigest()[:16]


def card(data, kind='codeblock', display=False):
    value = 'data:' + quote(json.dumps(data, ensure_ascii=False, separators=(',', ':')), safe='')
    return '<card type="' + ('block' if display else 'inline') + '" name="' + kind + '" value="' + value + '"></card>'


def code_card(key, code, mode, config):
    data = copy.deepcopy(config.get('cardDefaults', {}))
    data.update(copy.deepcopy(config.get('cards', {}).get(key, {})))
    data.update(code=code, mode=mode)
    data.setdefault('id', stable_id('code', key))
    data.setdefault('search', key.split('/')[-1])
    data.setdefault('collapsed', True)
    data.setdefault('name', data['search'])
    return card(data)


def build(source=None, config=None, require_all=True, collection='core'):
    """Return generated Markdown/Lake and provenance. Purely local; never publishes."""
    if collection not in ('core', 'extra'):
        raise ValueError('Unknown collection: ' + collection)
    source = (DOCUMENT if collection == 'core' else repository.EXTRA_DOCUMENT) if source is None else Path(source)
    config = settings(collection) if config is None else config
    text = source.read_text(encoding='utf-8')
    raw_text = text
    paths = catalog(collection=collection)
    source_hashes = {'Manifest.json': hashlib.sha256(repository.MANIFEST.read_bytes()).hexdigest()}

    def readme(match):
        name = match.group(1)
        item = next((v for v in paths.values() if v['module'] == name), None)
        if item is None:
            raise ValueError('找不到模块说明：' + name)
        path = item['path'].parent / 'README.md'
        body = path.read_text(encoding='utf-8')
        source_hashes[str(path.relative_to(ROOT))] = hashlib.sha256(path.read_bytes()).hexdigest()
        body = re.sub(r'\A# [^\n]*\n', '', body).strip()
        # Repository-relative links are meaningful in the module README. The
        # published document keeps their labels; external links stay clickable.
        body = re.sub(r'\[([^\]]+)\]\(([^)]+)\)',
                      lambda m: m[0] if '://' in m[2] or m[2].startswith('#') else m[1], body)
        body = re.sub(r'^(#{2,5}) ', r'\1# ', body, flags=re.M)
        return body

    md = markdown_parser()
    lines = text.splitlines(keepends=True)
    for token in reversed(md.parse(text)):
        if token.type == 'html_block':
            match = re.fullmatch(r'<!--\s*@readme\s+([^\s]+)\s*-->', token.content.strip())
            if match:
                start, end = token.map
                lines[start:end] = [readme(match) + '\n']
    text = ''.join(lines)
    tokens = md.parse(text)
    seen, replacements, heading_ids = set(), [], set()
    headings, example_counts = [], {}
    # Only standalone HTML comment tokens are directives. Examples in fences stay literal.
    for i, token in enumerate(tokens):
        if token.type == 'html_block':
            match = MARKER.fullmatch(token.content.strip())
            if not match:
                raise ValueError('不支持的 HTML 或指令，行 ' + str(token.map[0] + 1))
            kind, key = match.groups()
            if kind == 'code':
                if key not in paths:
                    raise ValueError('找不到模板：' + key)
                if key in seen:
                    raise ValueError('重复模板引用：' + key)
                seen.add(key)
                path = paths[key]['path']
                code, mode = code_of(path), 'cpp'
                sources = [path]
            else:
                mode = 'python'
                if key == 'CTL':
                    path = ROOT / 'ctl'
                    code = path.read_text(encoding='utf-8')
                    sources = [path]
                elif key == 'SnippetInstaller':
                    if collection != 'core':
                        raise ValueError('extra does not generate snippets or installers')
                    from tools.Fetcher import standalone
                    result = standalone()
                    code, sources = result['code'], result['sources']
                    sources = sources + [ROOT / 'tools/Library.py']
                else:
                    raise ValueError('未知工具引用：' + key)
                key = 'tool/' + key
            for path in sources:
                source_hashes[str(path.relative_to(ROOT))] = hashlib.sha256(path.read_bytes()).hexdigest()
            token.type, token.tag, token.content = 'fence', 'code', code
            token.info = mode
            token.meta = {'key': key}
            fence = '`' * max(3, max((len(s) for s in re.findall(r'`+', code)), default=0) + 1)
            replacements.append((token.map, fence + mode + '\n' + code.rstrip('\n') + '\n' + fence + '\n'))
        elif token.type == 'heading_open':
            title = tokens[i + 1].content.strip()
            level = int(token.tag[1])
            headings = [entry for entry in headings if entry[0] < level]
            headings.append((level, title))
            key = '/'.join(entry[1] for entry in headings)
            identity = config.get('headings', {}).get(key, stable_id('h', key))
            if identity in heading_ids:
                raise ValueError('标题重复，需要不同的分层名：' + key)
            heading_ids.add(identity)
            token.attrSet('id', identity)
            token.attrSet('data-lake-id', identity)
        elif token.type == 'fence':
            section = '/'.join(entry[1] for entry in headings) or 'Example'
            example_counts[section] = example_counts.get(section, 0) + 1
            token.meta = {'key': 'example/' + str(example_counts[section]) + '/' + section}
            token.info = token.info or 'plain'
    if require_all and seen != set(paths):
        raise ValueError('文档未引用模板：' + ', '.join(sorted(set(paths) - seen)))
    # Reject raw inline HTML too; migration removes Yuque font styling, not text.
    for token in tokens:
        if token.children and any(child.type == 'html_inline' for child in token.children):
            raise ValueError('正文不支持原始 HTML；请使用 Markdown 语法')

    def fence_rule(renderer, items, idx, options, env):
        token = items[idx]
        return code_card(token.meta['key'], token.content, token.info.strip(), config) + '\n'

    math_counts = {}

    def math_render(renderer, items, idx, options, env):
        token = items[idx]
        count = math_counts.get(token.content, 0)
        math_counts[token.content] = count + 1
        previous = config.get('mathIds', {}).get(token.content, [])
        identity = previous[count] if count < len(previous) else stable_id('math', token.content + ':' + str(count))
        data = {'code': token.content, 'id': identity,
                'src': 'https://www.yuque.com/api/services/graph/generate_redirect/latex?' + quote(token.content, safe='')}
        return card(data, 'math', token.meta.get('display', False))

    md.add_render_rule('fence', fence_rule)
    md.add_render_rule('math', math_render)
    rendered = md.renderer.render(tokens, md.options, {})
    lake = ('<!doctype lake><meta name="doc-version" content="1" />'
            '<meta name="typography" content="classic" />\n' + rendered)
    lines = text.splitlines(keepends=True)
    for (start, end), value in reversed(replacements):
        lines[start:end] = [value]
    expanded = ''.join(lines)
    provenance = {'collection': collection, 'templates': len(seen), 'sourceSha256': source_hashes,
                  'documentSha256': hashlib.sha256(raw_text.encode()).hexdigest(),
                  'configSha256': hashlib.sha256(json.dumps(config, sort_keys=True).encode()).hexdigest()}
    return {'markdown': expanded, 'lake': lake, 'provenance': provenance}
