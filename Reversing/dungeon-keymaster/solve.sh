#!/bin/bash

URL=http://localhost:8734

if [[ -n "$1" ]]; then
  URL="$1"
fi

# solve the dungeon.
curl $URL/dungeon?key=/dungeon-8734-http://localhost:8734
