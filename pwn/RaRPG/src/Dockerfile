FROM ubuntu:20.04 AS stage1
COPY . /src
WORKDIR /src
RUN apt update && apt install -y cmake libprotobuf-dev libenet-dev libncurses-dev clang make protobuf-compiler
RUN rm -f CMakeCache.txt
RUN cp /usr/lib/x86_64-linux-gnu/libenet.so.7 /usr/lib/x86_64-linux-gnu/libprotobuf.so.17 /usr/lib/x86_64-linux-gnu/libncurses.so.6 /usr/lib/x86_64-linux-gnu/libtinfo.so.6 .
RUN cmake build . && make

FROM scratch AS export
COPY --from=stage1 /src/client /src/server /src/lib*.so* /
