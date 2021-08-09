#!/bin/bash
docker rm -f sss
docker build --tag=sss . && \
docker run -p 1337:1337 --restart=on-failure --name=sss --detach sss
