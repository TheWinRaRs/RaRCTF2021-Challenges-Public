#!/bin/bash
docker rm -f rotorsa
docker build --tag=rotorsa . && \
docker run -p 1337:1337 --restart=on-failure --name=rotorsa --detach rotorsa
