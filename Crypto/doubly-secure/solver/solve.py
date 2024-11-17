#!/usr/bin/env python3


import websocket
import re
import os
import subprocess
import requests
import time
import sys

ADDRESS = os.getenv("ADDRESS", "localhost")
PORT = os.getenv("PORT", 8080)


def extract_key(data):
    keyR = re.compile(r"<pre>\s*(.*)\s*<\/pre>")
    matches  = keyR.search(data)
    if matches:
        return matches.group(1).split(" ")[-1]
    else:
        return None


def generate_key() -> tuple[str, str]:
    cmd = "age-keygen"
    res = subprocess.run(cmd, shell=True, capture_output=True, text=True).stdout

    pub_key = re.search(r"public key: (.*)", res)
    if pub_key:
        pub_key = pub_key.group(1)
    else:
        pub_key = ""
    priv_key = re.search(r"(AGE-SECRET-KEY-.*)", res)
    if priv_key:
        priv_key = priv_key.group(1)
    else:
        priv_key = ""

    return pub_key, priv_key


def age_encrypt(key: str, message: str) -> bytes:
    cmd = f"echo {message} | age -r {key} -e -o message.txt"
    res = subprocess.run(cmd, shell=True)
    with open("message.txt", "rb") as f:
        val = f.read()
        print(f"[{res}] Encryption Result: {val}")
        return val


def age_decrypt(priv_key: str, message: bytes) -> str:
    with open("message.txt", "wb") as f:
        f.write(message)
    with open("key.txt", "w") as f:
        f.write(priv_key)
    cmd = f"age -d -i key.txt -o decrypt.txt message.txt"
    res = subprocess.run(cmd, shell=True)
    print(f"[*] Decryption Result: {res}")
    with open("decrypt.txt", "r") as f:
        return f.read()


def main():
    # Generate a new key pair
    AGE_PUBKEY, AGE_PRIVKEY = generate_key()
    print(f"[*] Generated key pair: {AGE_PUBKEY} {AGE_PRIVKEY}")

    # Get the age public key from the website
    res = requests.get(f"http://{ADDRESS}:{PORT}/")
    key = extract_key(res.text)
    if key is None:
        print("[!] Failed to extract key")
        return
    print(f"[*] Extracted key: {key}")


    # Generate a message containing the age public key and
    # send it encrypted with recipient key
    res = age_encrypt(key, AGE_PUBKEY)

    print("[*] Sending encrypted message")
    ws = websocket.WebSocket()
    ws.connect(f"ws://{ADDRESS}:{PORT}/ws")
    with open("message.txt", "rb") as f:
        ws.send_binary(f.read())

    res = ws.recv()
    print(f"[*] Received: {res}")

    # De-Base64 the received message
    import base64

    res = base64.b64decode(res)

    # Decrypt the received message
    res = age_decrypt(AGE_PRIVKEY, res)
    print(f"[*] Decrypted message: {res}")
    return res.split(" ")[-1].strip()


if __name__ == "__main__":
    actual_flag = "sun{ag3_p1u5_tls_equalz_w3bs0ck3ts}"
    try:
        flag = main()
        if flag != actual_flag:
            print(f"[-] Flag is incorrect <{flag}>")
            sys.exit(1)
        else:
            print(f"[+] Flag is correct: <{flag}>")
    except Exception as e:
        print(f"[-] Error: {e}")
