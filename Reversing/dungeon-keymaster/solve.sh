#!/bin/bash
# Solve: the key is "<q1>-<q2>-<q3>" = /dungeon-8734-http://localhost:8734.
# Reads $URL from the environment (set by `pwnmake check`); $1 overrides.
URL="${URL:-http://localhost:24101}"
if [[ -n "$1" ]]; then
  URL="$1"
fi

# solve the dungeon.
curl -s "$URL/dungeon?key=/dungeon-8734-http://localhost:8734"
echo
