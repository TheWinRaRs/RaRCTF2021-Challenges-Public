#!/bin/bash
docker rm -f unrandompad
docker build --tag=unrandompad . && \
docker run -p 1002:1337 --restart=on-failure --name=unrandompad --detach unrandompad
