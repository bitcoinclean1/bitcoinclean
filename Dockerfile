FROM ubuntu:14.04

RUN apt-get update
RUN apt-get install -y autoconf automake autotools-dev libsigsegv2 m4 libtool pkg-config

RUN apt-get install -y build-essential
RUN apt-get install -y git
RUN apt-get install -y software-properties-common python-software-properties
RUN apt-get install -y libdb++-dev
RUN apt-get install -y bsdmainutils
RUN apt-get install -y libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev libboost-all-dev
RUN apt-get install -y libssl-dev
RUN apt-get install -y openssl libcurl4-openssl-dev libsasl2-dev
RUN apt-get install -y libevent-dev

RUN add-apt-repository ppa:bitcoin/bitcoin
RUN apt-get update
RUN apt-get install -y libdb4.8-dev libdb4.8++-dev

RUN git clone https://github.com/sipa/bitcoin-seeder.git
RUN cd /bitcoin-seeder ; make

COPY . /btc_clean_sandbox
COPY ./conf/bitcoin.conf /root/.bitcoin/bitcoin.conf

RUN cd /btc_clean_sandbox ; /bin/bash ./autogen.sh
RUN cd /btc_clean_sandbox ; ./configure
RUN cd /btc_clean_sandbox ; chmod 777 share/genbuild.sh
RUN cd /btc_clean_sandbox ; make
RUN cd /btc_clean_sandbox ; make install