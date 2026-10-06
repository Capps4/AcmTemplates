"""Scan active C++ template headers; semantic candidates require explicit review."""
from collections import defaultdict
import hashlib
import json
from pathlib import Path
import re
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tools.Common import FLAGS, ROOT, digest, manifest, module_path, catalog

REVIEWS = ROOT / 'tools/RuleExceptions.json'
REQUIRED = '-std=gnu++17 -Wall -Wextra -Werror -Weffc++ -O0 -g -D_GLIBCXX_DEBUG'.split()
KEYWORDS = set('''alignas alignof and and_eq asm auto bitand bitor bool break case catch
char char16_t char32_t class compl const constexpr const_cast continue decltype default
delete do double dynamic_cast else enum explicit export extern false float for friend goto
if inline int long mutable namespace new noexcept not not_eq nullptr operator or or_eq
private protected public register reinterpret_cast return short signed sizeof static
static_assert static_cast struct switch template this thread_local throw true try typedef
typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq'''.split())
# Conventional library/protocol operations are not short-name candidates. Own helpers
# such as canTransform and materialize are still reported even when they look like APIs.
PROTOCOL = set('''assert begin end rbegin rend cbegin cend crbegin crend size empty clear
resize reserve capacity data front back push_back pop_back push_front pop_front emplace
emplace_back emplace_front insert erase assign fill swap find count lower_bound upper_bound
equal_range substr value value_or has_value reset get move forward make_pair make_tuple
tie invoke declval decay_t remove_cv_t remove_reference_t value_type iterator const_iterator
difference_type reference const_reference pointer const_pointer iterator_category std
numeric_limits iterator_traits common_type_t enable_if_t conditional_t void_t make_unsigned_t
tuple_element_t index_sequence make_index_sequence index_sequence_for remove_cvref_t
__int128 __int128_t __uint128_t __FILE__ __LINE__ __VA_ARGS__'''.split())
LEX = re.compile(
    r'//[^\n]*|/\*.*?\*/|(?:u8|u|U|L)?R"(?P<delim>[^\s()\\]{0,16})\(.*?\)(?P=delim)"'
    r'|(?:u8|u|U|L)?"(?:\\.|[^"\\])*"'
    r"|\b[0-9][A-Za-z0-9_'.]*|(?:u8|u|U|L)?'(?:\\.|[^'\\])*'", re.S)
WORDS = re.compile(r'[A-Za-z_]\w*|::|&&|\|\||[^\s]')
TRAITS = re.compile(r'(?:enable_if(?:_t)?|void_t|conditional(?:_t)?|is_\w+_v|is_convertible)')


def mask(text, literals=True):
    """Keep locations, removing comments/literals; digit separators stay numeric tokens."""
    def replace(match):
        value = match[0]
        if value[0].isdigit():
            return value
        if not literals and not value.startswith(('//', '/*')):
            return value
        return ''.join('\n' if c == '\n' else ' ' for c in value)
    return LEX.sub(replace, text)


def files():
    result = {item['path']: (item['module'], item['include']) for item in catalog().values()}
    for name, item in manifest().items():
        folder = module_path(name)
        for path in folder.glob('Include.hpp'):
            result[path] = (name, None)
    for path in (ROOT / 'Headers').glob('*.hpp'):
        result[path] = ('Headers', None)
    return result


def scan():
    findings = []
    metrics = {}
    inventory = files()
    for path, (module, expected) in inventory.items():
        rel = str(path.relative_to(ROOT))
        text = path.read_text()
        code = mask(text)
        lines = code.splitlines()
        groups = defaultdict(list)

        def add(rule, symbol, number, message, level='review'):
            groups[(rule, symbol, message, level)].append(number)

        directives = set()
        continued = False
        depth = 0
        packed = []
        for number, line in enumerate(lines, 1):
            if continued or line.lstrip().startswith('#'):
                directives.add(number)
                continued = line.rstrip().endswith('\\')
        for number, line in enumerate(lines, 1):
            tokens = WORDS.findall(line)
            for i, word in enumerate(tokens):
                if word == '||' and (i == 0 or tokens[i - 1] != 'operator'):
                    add('R07', '||', number, 'Use or for logical OR', 'error')
                elif word == '&&' and (i == 0 or tokens[i - 1] != 'operator'):
                    add('R07', '&&', number, 'Distinguish reference syntax from logical AND')
                if number in directives:
                    continue
                if word == 'typedef':
                    add('R08', 'typedef', number, 'Prefer using if the alias contract is unchanged')
                if TRAITS.fullmatch(word):
                    add('R09', word, number, 'Explain overload, representation or ownership requirement')
                if (re.fullmatch(r'[a-z_][A-Za-z0-9_]*', word) and len(word.lstrip('_')) > 4
                        and word not in KEYWORDS | PROTOCOL and not word.startswith('__')
                        and not (i >= 2 and tokens[i - 2:i] == ['std', '::'])
                        and not word.startswith('is_')):
                    add('R04', word, number, 'Review long internal name or preserve public/type contract')
            if number not in directives:
                # Semicolons inside parentheses include for headers, not packed statements.
                semis = 0
                for token in tokens:
                    if token == '(':
                        depth += 1
                    elif token == ')':
                        depth -= 1
                    elif token == ';' and depth == 0:
                        semis += 1
                if semis > 1:
                    packed.append(number)
                    add('R06', 'packed', number, 'Review multiple statements on one line')
                for i, word in enumerate(tokens):
                    if word not in ('if', 'for', 'while', 'else'):
                        continue
                    if word == 'while' and tokens[:i] == ['}']:
                        continue  # do-while tail has no packed body.
                    tail = tokens[i + 1:]
                    if tail[:1] == ['constexpr']:
                        tail = tail[1:]
                    if word != 'else':
                        if not tail or tail[0] != '(':
                            continue
                        nesting = 0
                        end = None
                        for j, token in enumerate(tail):
                            nesting += (token == '(') - (token == ')')
                            if nesting == 0:
                                end = j
                                break
                        if end is None:
                            continue
                        tail = tail[end + 1:]
                    if tail and tail[0] not in ('{', 'if', ';') and ';' in tail:
                        add('R05', 'body', number, 'Expand a control body on the same line')
        source = mask(text, literals=False).splitlines()
        eligible = sum(bool(line.strip()) and n not in directives for n, line in enumerate(source, 1))
        metrics[rel] = {'module': module, 'sourceLines': eligible, 'packedCandidateLines': packed,
                        'candidatePercent': round(100 * len(packed) / eligible, 2) if eligible else 0}
        includes = []
        for number, line in enumerate(text.splitlines(), 1):
            if not re.match(r'^\s*#\s*include\b', lines[number - 1]):
                continue
            match = re.match(r'^\s*#\s*include\s*"([^"]+)"', line)
            if match:
                includes.append(match[1])
        if expected is not None and (not includes or includes[0] != expected):
            add('R18', 'entry', 1, 'Expected first local include: ' + expected, 'error')
        if not re.search(r'^\s*#\s*pragma\s+once\b', code, re.M):
            add('R18', 'once', 1, 'Active header requires pragma once', 'error')
        for include in includes:
            target = (path.parent / include).resolve()
            if not target.is_relative_to(ROOT) or not target.is_file():
                add('R18', include, 1, 'Missing or external local include', 'error')
        for (rule, symbol, message, level), numbers in groups.items():
            positions = sorted(set(numbers))
            anchors = [' '.join(WORDS.findall(lines[n - 1])) for n in positions]
            fingerprint = hashlib.sha256('\n'.join(anchors).encode()).hexdigest()
            findings.append({'id': f'{rel}:{rule}:{symbol}', 'rule': rule, 'symbol': symbol,
                             'file': rel, 'module': module, 'line': positions[0], 'lines': positions,
                             'fingerprint': fingerprint, 'status': level, 'message': message})
    flags = FLAGS['strict']
    invalid = any(flag.startswith('-Wno-') or flag in ('-w', '-fpermissive', '-U_GLIBCXX_DEBUG')
                  or flag.startswith('-std=') and flag != '-std=gnu++17'
                  or flag.startswith('-O') and flag != '-O0' for flag in flags)
    if not set(REQUIRED).issubset(flags) or invalid:
        findings.append({'id': 'tools/__init__.py:R17:strict', 'rule': 'R17', 'symbol': 'strict',
                         'file': 'tools/__init__.py', 'line': 1, 'status': 'error',
                         'message': 'Required strict flags missing or warnings suppressed'})
    return inventory, findings, metrics


def reviewed(findings):
    data = json.loads(REVIEWS.read_text()) if REVIEWS.exists() else {'reviews': []}
    decisions = {}
    for entry in data['reviews']:
        if entry['id'] in decisions or not entry.get('reason', '').strip():
            raise ValueError('Duplicate or unexplained review: ' + entry['id'])
        decisions[entry['id']] = entry
    seen = set()
    for item in findings:
        entry = decisions.get(item['id'])
        if entry and item['status'] == 'review' and entry['fingerprint'] == item['fingerprint']:
            item.update(status='kept', reason=entry['reason'])
            seen.add(item['id'])
    for key in decisions.keys() - seen:
        findings.append({'id': key + ':stale', 'rule': 'R00', 'file': str(REVIEWS.relative_to(ROOT)),
                         'line': 1, 'status': 'error', 'message': 'Review expired; inspect or remove: ' + key})
    return findings


def run():
    """Strict scan; the CTL entry owns presentation and process exit status."""
    inventory, findings, metrics = scan()
    reviewed(findings)
    counts = {status: sum(f['status'] == status for f in findings) for status in ('error', 'review', 'kept')}
    result = {'scope': [str(p.relative_to(ROOT)) for p in inventory], 'counts': counts,
              'sha256': {str(p.relative_to(ROOT)): digest(p) for p in inventory},
              'ruleInputs': {str(p.relative_to(ROOT)): digest(p) for p in
                             (ROOT / 'AGENTS.md', ROOT / 'Docs/Rules.md', REVIEWS,
                              Path(__file__).resolve(), ROOT / 'Manifest.json',
                              ROOT / 'tools/Common.py', ROOT / 'tools/Catalog.py', ROOT / 'tools/__init__.py')
                             if p.is_file()},
              'metrics': metrics, 'findings': findings,
              'passed': counts['error'] == 0 and counts['review'] == 0}
    from tools import REPORTS
    from tools.Common import write_json
    write_json(REPORTS / 'Rules.json', result)
    return result
