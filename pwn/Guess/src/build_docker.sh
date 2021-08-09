#!/bin/sh

docker rm -f guessing
docker build . -t guessing
docker run -it --name guessing -p 1337:1337 guessing
