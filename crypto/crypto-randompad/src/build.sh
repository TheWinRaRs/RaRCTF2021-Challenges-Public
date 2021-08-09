#!/bin/bash
docker rm -f randompad
docker build --tag=randompad . && \
docker run -p 1004:1337 --restart=on-failure --name=randompad --detach randompad
