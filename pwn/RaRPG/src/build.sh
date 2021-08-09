#!/bin/sh

# This builds the challenge in a container and extracts the necessary runtime libraries for preloading for incompatible systems
rm -rf build
mkdir build
DOCKER_BUILDKIT=1 docker build . -o build

docker build . --file Docker-remote -t ctf:rarpg
