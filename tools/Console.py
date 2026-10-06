"""Human-readable CTL stages and progress; ANSI only on interactive terminals."""
from contextlib import contextmanager
import os
import sys
import time


class Console:
    def __init__(self, title, stream=None):
        self.stream = stream if stream is not None else sys.stdout
        self.color = bool(self.stream.isatty() and not os.environ.get('NO_COLOR')
                          and os.environ.get('TERM') != 'dumb')
        self.started = time.monotonic()
        self.title = title
        self.stages = 0
        self.write('\n' + self.paint('CTL', '36;1') + ' · ' + title + '\n')

    def paint(self, text, code):
        return '\033[' + code + 'm' + text + '\033[0m' if self.color else text

    def write(self, text):
        print(text, file=self.stream, flush=True)

    def note(self, text):
        for line in str(text).splitlines():
            self.write('    ' + line)

    @contextmanager
    def step(self, title):
        self.stages += 1
        label = '[' + str(self.stages) + '] ' + title
        self.write('  ' + self.paint('›', '36') + ' ' + label)
        start = time.monotonic()
        try:
            yield
        except Exception:
            self.write('  ' + self.paint('×', '31') + ' ' + label + '  ' + self.duration(start))
            raise
        self.write('  ' + self.paint('✓', '32') + ' ' + label + '  ' + self.duration(start))

    @staticmethod
    def duration(start):
        return f'{time.monotonic() - start:.2f}s'

    def progress(self, done, total, name, status, detail=None):
        width = len(str(total))
        success = status in ('passed', 'compiled', 'cached')
        symbol = '✓' if success else '×'
        label = {'passed': '通过', 'compiled': '已编译', 'cached': '复用缓存', 'failed': '失败'}.get(status, status)
        self.write('    ' + self.paint(symbol, '32' if success else '31') +
                   f' [{done:>{width}}/{total}] {name} · {label}')
        if detail:
            self.note(detail)

    def finish(self, text):
        self.write('\n' + self.paint('完成', '32;1') + ' · ' + text + ' · ' + self.duration(self.started))

    def error(self, error):
        self.write('\n' + self.paint('失败', '31;1') + ' · ' + str(error))
        self.write('耗时 ' + self.duration(self.started))
