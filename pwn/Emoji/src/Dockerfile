from ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y socat
RUN useradd -ms /bin/bash ctf
EXPOSE 1337
COPY emoji /emoji
COPY flag.txt /flag.txt
USER ctf
CMD ["socat", "tcp-l:1337,reuseaddr,fork", "EXEC:/emoji"]
