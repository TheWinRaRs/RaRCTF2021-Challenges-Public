FROM ubuntu:18.04
RUN apt-get update -y && apt-get install -y \
    lib32z1 xinetd \
&& rm -rf /var/lib/apt/lists/*
RUN useradd ctf
RUN mkdir /ctf
RUN echo "You've been blocked by our xinetd - try again, and report if this repeats." > /etc/banner_fail
COPY ./ctf.xinetd /etc/xinetd.d/ctf
COPY ./start.sh /start.sh
COPY ./setup.sh /setup.sh
COPY ./IronOxide /ctf/IronOxide
COPY ./elemprops.csv /ctf/elemprops.csv
COPY ./flag.txt /ctf/flag.txt
RUN chown -R root:ctf /ctf && chmod -R 750 /ctf
RUN chmod +x /setup.sh
RUN chown root:ctf /start.sh && chmod 750 /start.sh

CMD ["/setup.sh"]

EXPOSE 8888