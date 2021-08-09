#!/bin/bash
docker rm -f electroplating
docker build --tag=electroplating . && \
docker run -p 1013:1337 --restart=on-failure --name=electroplating --detach electroplating

