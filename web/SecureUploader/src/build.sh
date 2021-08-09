#!/bin/bash
docker rm -f secureuploader
docker build --tag=secureuploader . && \
docker run -p 1011:1337 --restart=on-failure --name=secureuploader --detach secureuploader
