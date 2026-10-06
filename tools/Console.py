"""Human-readable CTL stages and progress; ANSI only on interactive terminals."""
from contextlib import contextmanager
import os
import sys
import threading
import time
import unicodedata


class Console:
    def __init__(self, title, stream=None):
        self.stream = stream if stream is not None else sys.stdout
        self.color = bool(self.stream.isatty() and not os.environ.get('NO_COLOR')
                          and os.environ.get('TERM') != 'dumb')
        self.started = time.monotonic()
        self.title = title
        self.stages = 0
        self.transient = False
        self.lock = threading.RLock()
        self.running = None
        self.tasks = {}
        self.progress_started = self.started
        self.last_progress = 0
        self.write('\n' + self.paint('CTL', '36;1') + ' · ' + title + '\n')

    def paint(self, text, code):
        return '\033[' + code + 'm' + text + '\033[0m' if self.color else text

    def write(self, text):
        with self.lock:
            self.clear_progress()
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

    def clear_progress(self):
        with self.lock:
            if self.transient:
                print('\r\033[2K', end='', file=self.stream, flush=True)
                self.transient = False

    @contextmanager
    def track(self, name):
        """Keep long synchronous work visible, including tool tests and signatures."""
        stopped = threading.Event()
        with self.lock:
            self.tasks.clear()
            self.running = None
            self.progress_started = time.monotonic()
        self.progress(0, 0, name, 'running')

        def heartbeat():
            while not stopped.wait(5):
                with self.lock:
                    if self.running is not None:
                        self.render_running()

        reporter = threading.Thread(target=heartbeat, daemon=True)
        reporter.start()
        try:
            yield
        finally:
            stopped.set()
            reporter.join()
            with self.lock:
                self.running = None
                self.tasks.clear()
                self.clear_progress()

    def task(self, name, phase):
        """Workers update state only; the reporter owns periodic output."""
        with self.lock:
            if phase is None:
                self.tasks.pop(name, None)
            else:
                self.tasks[name] = phase

    def render_running(self):
        done, total, name, detail = self.running
        count = ' · 已完成 ' + str(done) + '/' + str(total) if total else ''
        text = name + count + ' · 已用 ' + self.duration(self.progress_started)
        active = [name + '：' + phase for name, phase in sorted(self.tasks.items())]
        if active:
            text += ' · 当前 ' + '；'.join(active[:3])
            if len(active) > 3:
                text += ' 等 ' + str(len(active)) + ' 个任务'
        elif detail:
            text += ' · ' + str(detail)
        if self.color:
            # Keep ANSI refresh on one physical line, including wide Chinese text.
            try:
                columns = os.get_terminal_size(self.stream.fileno()).columns
            except (AttributeError, OSError, ValueError):
                columns = 80
            budget = max(0, columns - 5)
            widths = [0 if unicodedata.combining(c) else
                      2 if unicodedata.east_asian_width(c) in ('W', 'F') else 1 for c in text]
            if sum(widths) > budget:
                used, end = 0, 0
                for width in widths:
                    if used + width > max(0, budget - 3):
                        break
                    used += width
                    end += 1
                text = text[:end] + '.' * min(3, budget)
            print('\r\033[2K    ' + text, end='', file=self.stream, flush=True)
            self.transient = True
        else:
            self.write('    ' + text)
        self.last_progress = time.monotonic()

    def progress(self, done, total, name, status, detail=None):
        if status == 'running':
            with self.lock:
                previous = self.running
                self.running = (done, total, name, detail)
                if previous is None or previous[2] != name or time.monotonic() - self.last_progress >= 5:
                    self.render_running()
            return
        self.clear_progress()
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
        self.clear_progress()
        self.write('\n' + self.paint('失败', '31;1') + ' · ' + str(error))
        self.write('耗时 ' + self.duration(self.started))
