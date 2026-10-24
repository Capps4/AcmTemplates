// Keep the pinned official CLI's requests, retries, output and exit codes.
// Its error formatter otherwise discards the HTTP response diagnostics.
import { pathToFileURL } from 'node:url';

const cli = pathToFileURL(process.argv[2]);
process.argv.splice(1, 1);
const { YuqueHttp } = await import(new URL('./client/http.js', cli));
const normalizeError = YuqueHttp.prototype.normalizeError;
const sensitive = /token|authorization|cookie|password|secret|credential|^body(?:_lake|_draft)?$/i;
const safeHeader = /^(?:date|server|content-type|content-length|retry-after|via|x-cache|(?:x-)?rate-?limit(?:-.*)?|(?:x-)?(?:request|correlation|trace|log)-?id|traceparent|x-tt-logid)$/i;
const secrets = [process.env.YUQUE_TOKEN, process.env.YUQUE_PERSONAL_TOKEN]
    .filter(Boolean).flatMap(value => [value, encodeURIComponent(value)]);

function redact(text) {
    for (const secret of secrets) text = text.split(secret).join('[REDACTED]');
    text = text.replace(/((?:["']?(?:[\w-]*token|authorization|proxy-authorization|set-cookie|cookie|password|secret|credential)["']?)\s*[:=]\s*)(?:"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|[^\r\n]+)/gi,
        '$1[REDACTED]');
    return text.replace(/[\x00-\x08\x0b-\x1f\x7f]/g,
        char => '\\u' + char.charCodeAt(0).toString(16).padStart(4, '0'));
}

function format(value) {
    const text = JSON.stringify(value, (key, item) => sensitive.test(key) ? '[REDACTED]' : item, 2);
    const safe = redact(text ?? '(empty)');
    return safe.length <= 32768 ? safe : safe.slice(0, 32768) + '\n[truncated at 32768 characters]';
}

YuqueHttp.prototype.normalizeError = function (error, method) {
    const normalized = normalizeError.call(this, error, method);
    normalized.message = redact(normalized.message);
    try {
        const response = error?.response;
        const request = error?.config;
        const details = [];
        if (request) {
            const url = new URL(this.axios.getUri(request));
            details.push('Request: ' + format(method.toUpperCase() + ' ' + url.origin + url.pathname));
        }
        if (error?.code) details.push('Client code: ' + format(error.code));
        if (response) {
            details.push('HTTP: ' + response.status + ' ' + format(response.statusText ?? ''));
            const headers = Object.fromEntries(Object.entries(response.headers ?? {})
                .filter(([key]) => safeHeader.test(key) && !sensitive.test(key)));
            details.push('Response headers (safe fields): ' + format(headers));
            details.push('Response body: ' + format(response.data));
        } else {
            details.push('HTTP: no response received');
        }
        normalized.message += '\n' + details.join('\n');
    } catch {
        normalized.message += '\nHTTP diagnostics could not be formatted.';
    }
    return normalized;
};

await import(cli.href);
