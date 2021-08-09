#!/bin/bash
docker rm -f babycrypt
docker build --tag=babycrypt . && \
docker run -p 1003:1337 --restart=on-failure --name=babycrypt --detach babycrypt
