# unrandompad
FROM python:3.9.2-slim-buster

RUN apt-get update && apt-get install -y \
  xinetd \
  && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir pycryptodome

RUN mkdir -p /challenge
RUN useradd -M -d /challenge ctf
WORKDIR /challenge

COPY ctf.xinetd /etc/xinetd.d/ctf
COPY . /challenge/

RUN chown -R ctf:ctf /challenge && chmod -R 770 /challenge
RUN chown -R root:ctf /challenge && \
  chmod -R 750 /challenge

CMD ["/usr/sbin/xinetd", "-dontfork"]

EXPOSE 1337
