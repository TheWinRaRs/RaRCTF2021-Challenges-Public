import requests

import hashlib
import uuid
import binascii
import os
import sys

def generate():
    return uuid.uuid4().hex[:4], uuid.uuid4().hex[:4]

def verify(prefix, suffix, answer, difficulty=6):
    hash = hashlib.sha256(prefix.encode() + answer.encode() + suffix.encode()).hexdigest()
    return hash.endswith("0"*difficulty)

def solve(prefix, suffix, difficulty):
    while True:
        test = binascii.hexlify(os.urandom(4)).decode()
        if verify(prefix, suffix, test, difficulty):
            return test

if len(sys.argv) < 2:
    print("Usage: solve.py http://host:port/")
    exit()

s = requests.Session()
host = sys.argv[1]
data = s.get(host + "pow").json()
print("Solving POW")
solution = solve(data['pref'], data['suff'], 6)
print(f"Solved: {solution}")

s.post(host + "pow", json={"answer": solution})

r = s.post(host, files={"file": open('solve.htmlrs', 'rb')})
# r = s.post(host, files={"file": open('src/template.htmlrs', 'rb')})
print(r.text)
try:
    print(r.text.split('\n')[14])
except:
    print("Blocked")

