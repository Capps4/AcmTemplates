"""The only public command surface for Capps Template Library."""
import argparse
import platform

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
任意工作目录均可调用；产物、日志和报告保存在仓库 .cache/。
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
    return result


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
    errors = []
    counts = []
    # Continue the second category even if the first has failed; each saves its report.
    for enabled, title, module in [(right, '正确性测试', 'Run'), (perf, '性能测试', 'Benchmark')]:
        if not enabled:
            continue
        try:
            with console.step(title):
                import importlib
                runner = importlib.import_module('tools.' + module)
                result = runner.run(config.TEST_MODULES, progress=console.progress)
                records = result['records']
                failed = sum(r['status'] != 'passed' for r in records)
                cached = sum(bool(r.get('cached')) for r in records)
                counts.append(f'{title} {len(records) - failed}/{len(records)} 通过')
                console.note(f'{len(records) - failed} 个通过 · {failed} 个失败 · {cached} 个复用缓存')
                console.note('报告：' + result['path'])
                if not result['passed']:
                    raise ValueError(title + '存在失败任务，请查看日志')
        except (OSError, RuntimeError, ValueError) as exc:
            errors.append(str(exc))
    with console.step('汇总当前验证报告'):
        from tools.Verification import save
        evidence, path = save()
        console.note('报告：' + str(path))
        console.note('全库发布验证：' + ('通过' if evidence['passed'] else '未完成；缺失或过期记录不能算通过'))
    if errors:
        raise ValueError('\n'.join(errors))
    console.finish(' · '.join(counts))
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
            return test(console, args.right or args.all, args.perf or args.all)
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
