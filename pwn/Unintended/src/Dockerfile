FROM ubuntu:18.04
RUN apt-get update -y && apt-get install -y \
    lib32z1 xinetd \
 && rm -rf /var/lib/apt/lists/*
RUN useradd clubby
RUN mkdir /pwn
RUN echo "You've been blocked by our xinetd - try again, and report if this repeats." > /etc/banner_fail
COPY ./ctf.xinetd /etc/xinetd.d/pwn
COPY ./start.sh /start.sh
COPY ./setup.sh /setup.sh
COPY ./unintended /pwn/unintended
COPY ./flag_0eff9e0ad1.txt /pwn/flag_0eff9e0ad1.txt
COPY ./lib /pwn/lib
RUN chown -R root:clubby /pwn && chmod -R 750 /pwn
RUN chmod +x /setup.sh
RUN chown root:clubby /start.sh && chmod 750 /start.sh

CMD ["/setup.sh"]

EXPOSE 8888