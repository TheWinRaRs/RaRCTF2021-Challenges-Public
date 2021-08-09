#!/bin/bash
docker rm -f boring-flag-runner
docker build --tag=boring-flag-runner . && \
docker run -p 1001:1337 --restart=on-failure --name=boring-flag-runner --detach boring-flag-runner
