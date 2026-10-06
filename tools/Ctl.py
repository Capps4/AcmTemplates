"""The only public command surface for Capps Template Library."""
import argparse
import platform
import re
import shutil
import time
from contextlib import contextmanager

import tools as config
from tools.Console import Console


HELP = '''Capps Template Library

  ctl update --yuque   更新语雀文档
  ctl update --local   安装本地 VS Code snippets
  ctl rule            严格检查模板规则
  ctl test --right    正确性测试（严格编译 / 优化 / Sanitizer）
  ctl test --perf     性能测试
  ctl test --all      正确性和性能测试

配置：tools/__init__.py；Token：.yuque/token 或 YUQUE_TOKEN。
任意工作目录均可调用；测试报告：.cache/reports/Verification.md；编译缓存：.cache/compiled/。
'''


def parser():
    result = argparse.ArgumentParser(prog='ctl', description=HELP,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    commands = result.add_subparsers(dest='command', required=True)
    update = commands.add_parser('update', help='更新语雀文档或本地 snippets')
    group = update.add_mutually_exclusive_group(required=True)
    group.add_argument('--yuque', action='store_true', help='更新语雀文档')
    group.add_argument('--local', action='store_true', help='安装 VS Code snippets')
    commands.add_parser('rule', help='严格检查模板规则')
    test = commands.add_parser('test', help='正确性或性能测试')
    group = test.add_mutually_exclusive_group(required=True)
    group.add_argument('--right', action='store_true', help='正确性测试')
    group.add_argument('--perf', action='store_true', help='性能测试')
    group.add_argument('--all', action='store_true', help='两种测试都执行')
    test.add_argument('--module', action='append', help='选择模块，可重复传入')
    test.add_argument('--case', help='复现 case：完整 ID 或模块内名称（正确性）')
    test.add_argument('--profile', choices=('strict', 'optimized', 'sanitized'), help='局部编译模式（正确性）')
    test.add_argument('--seed', type=int, help='复现随机种子')
    return result


@contextmanager
def test_lock():
    import fcntl
    config.CACHE.mkdir(parents=True, exist_ok=True)
    # Keep the inode stable while other invocations are waiting for the lock.
    with (config.CACHE / '.test.lock').open('a') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        try:
            # Only our interrupted-run directories, after acquiring the whole-run lock.
            for path in config.CACHE.iterdir():
                if re.fullmatch(r'test-(?:(?:perf|build)-)?[a-z0-9_]{8}', path.name) and path.is_dir() and not path.is_symlink():
                    shutil.rmtree(path)
            yield
        finally:
            fcntl.flock(lock, fcntl.LOCK_UN)


def update_local(console):
    from tools.Fetcher import snippets, locate, plan, install
    with console.step('读取模板清单'):
        count = len(snippets())
        console.note(str(count) + ' 个模板')
    with console.step('定位 VS Code 用户目录'):
        directory = locate(config.SNIPPETS_DIR, config.VSCODE_USER_DATA)
        console.note(directory)
    with console.step('检查重复前缀与旧模板'):
        legacy = plan(directory)
        console.note(str(len(legacy)) + ' 个旧模板将先备份再迁移' if legacy else '未发现旧模板或重复前缀')
    with console.step('备份并安装'):
        result = install(directory, migrate=config.MIGRATE_LEGACY_SNIPPETS)
        console.note('安装位置：' + result['installed'])
        if result.get('backup'):
            console.note('备份位置：' + result['backup'])
    console.finish(str(count) + ' 个模板已更新' if result['changed'] else '模板已是最新，无需写入')
    return 0


def rule(console):
    from tools.CheckRules import run
    with console.step('扫描模板与头文件'):
        result = run()
        counts = result['counts']
        console.note(f"{len(result['scope'])} 个头文件 · {counts['error']} 个错误 · "
                     f"{counts['review']} 个待审查 · {counts['kept']} 个已说明保留项")
        for item in result['findings']:
            if item['status'] != 'kept':
                console.note(f"{item['file']}:{item['line']} [{item['rule']}] "
                             + item.get('reason', item['message']))
        console.note('报告：' + str(config.REPORTS / 'Rules.json'))
        if not result['passed']:
            raise ValueError('规则检查未通过，请处理错误、待审查项或过期保留项')
    console.finish('规则检查通过')
    return 0


def test(console, right, perf):
    if platform.system() not in ('Darwin', 'Linux'):
        raise ValueError('当前测试执行器需要 macOS 或 Linux；Windows 可使用 WSL')
    from tools.Common import catalog
    from tools.Verification import failure_text, save
    results, errors = [], []
    for enabled, title, module in [(right, '正确性', 'Run'), (perf, '性能', 'Benchmark')]:
        if not enabled:
            continue
        console.note(title + '：编译与测试中…')
        try:
            started = time.monotonic()
            import importlib
            with console.track(title + '：准备测试任务'):
                result = importlib.import_module('tools.' + module).run(
                    config.TEST_MODULES, progress=console.progress, activity=console.task)
            results.extend(result['records'])
            cached = sum(r.get('compileCached', False) for r in result['records'])
            compiled = sum('compileCached' in r for r in result['records'])
            console.note(title + '耗时：' + format(time.monotonic() - started, '.2f') +
                         's · 编译缓存 ' + str(cached) + '/' + str(compiled))
            if not result['passed']:
                errors.append(title + '测试失败')
        except (OSError, RuntimeError, ValueError) as exc:
            errors.append(title + '：' + str(exc))
    console.clear_progress()
    selected = set(config.TEST_MODULES) if config.TEST_MODULES else None
    items = [(key, item) for key, item in sorted(catalog().items())
             if selected is None or item['module'] in selected]
    for done, (key, item) in enumerate(items, 1):
        records = [r for r in results if r['module'] == item['module']]
        ok = bool(records) and all(r['status'] in ('passed', 'skipped') for r in records)
        if config.TEST_CASE:
            ok = ok and any(r.get('caseIds') for r in records)
        label = key + ('（无需性能测试）' if any(r['status'] == 'skipped' for r in records) else '')
        console.progress(done, len(items), label, 'passed' if ok else 'failed')
    tools = [r for r in results if r['module'] == 'Tools']
    if tools:
        console.note('CTL 工具回归：' + ('✓' if all(r['status'] == 'passed' for r in tools) else '×'))
    for record in results:
        if record['status'] not in ('passed', 'skipped'):
            console.note(failure_text(record))
    with console.track('汇总：检查并保存报告'):
        _, path = save()
    console.note('报告：' + str(path))
    if errors:
        raise ValueError('；'.join(errors))
    scope = '局部测试' if config.TEST_CASE or set(config.RIGHT_PROFILES) != {'strict', 'optimized', 'sanitized'} or selected else '测试'
    console.finish(str(len(items)) + ' 个模板' + scope + '完成')
    return 0


def main(argv=None):
    args = parser().parse_args(argv)
    title = {'rule': '规则检查', 'test': '模板测试', 'update': '更新模板'}[args.command]
    if args.command == 'update':
        title = '更新语雀文档' if args.yuque else '更新本地模板'
    console = Console(title)
    try:
        if args.command == 'rule':
            return rule(console)
        if args.command == 'test':
            if platform.system() not in ('Darwin', 'Linux'):
                raise ValueError('当前测试执行器需要 macOS 或 Linux；Windows 可使用 WSL')
            if (args.case or args.profile) and not (args.right or args.all):
                raise ValueError('--case/--profile 需要 --right 或 --all')
            if args.seed is not None and args.seed < 0:
                raise ValueError('种子须为非负整数')
            fields = ('TEST_MODULES', 'TEST_CASE', 'RIGHT_PROFILES', 'RIGHT_SEED', 'PERF_SEED')
            previous = {key: getattr(config, key) for key in fields}
            try:
                if args.module: config.TEST_MODULES = tuple(args.module)
                if args.case: config.TEST_CASE = args.case
                if args.profile: config.RIGHT_PROFILES = (args.profile,)
                if args.seed is not None: config.RIGHT_SEED = config.PERF_SEED = args.seed
                with test_lock():
                    return test(console, args.right or args.all, args.perf or args.all)
            finally:
                for key, value in previous.items(): setattr(config, key, value)
        if args.local:
            return update_local(console)
        from tools.Publisher import publish
        result = publish(console)
        console.finish('语雀文档已更新并验证' if result['applied'] else '语雀文档已是最新，无需写入')
        return 0
    except KeyboardInterrupt:
        console.error('已中断；语雀写入若已开始，请先核对远端和备份再重试'
                      if args.command == 'update' and args.yuque else '已中断')
        return 130
    except Exception as exc:
        console.error(exc)
        return getattr(exc, 'code', 1)
