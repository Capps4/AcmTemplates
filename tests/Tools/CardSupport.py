"""Read generated native cards for offline regression assertions."""
import html
import json
import re
from urllib.parse import unquote

CARD = re.compile(r'<card\b[^>]*\bname="codeblock"[^>]*>.*?</card>', re.S)


def decode(card):
    value = re.search(r'\bvalue="([^"]*)"', card)
    if not value:
        raise ValueError('Code card lacks value')
    encoded = html.unescape(value[1])
    if not encoded.startswith('data:'):
        raise ValueError('Unknown code card encoding')
    return json.loads(unquote(encoded[5:]))
