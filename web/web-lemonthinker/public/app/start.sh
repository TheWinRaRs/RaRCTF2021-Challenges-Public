#!/bin/sh
chmod 777 /app/static/images
gunicorn --chdir /app app:app -w 4 --threads 4 -b 0.0.0.0:1337
