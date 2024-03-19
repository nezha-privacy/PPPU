FROM ubuntu:20.04

ENV TZ=Asia/Shanghai \
    DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y apt-file
RUN apt-get install -y software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test

RUN apt-get update
RUN apt-get upgrade
RUN apt-get install -y build-essential
RUN apt-get install -y gcc-13
RUN apt-get install -y g++-13
RUN apt-get install -y python-dev
RUN apt-get install -y autotools-dev
RUN apt-get install -y cmake
RUN apt-get install -y wget
RUN apt-get install -y git
RUN apt-get install -y libicu-dev
RUN apt-get install -y libbz2-dev
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y libssl-dev
RUN apt-get install -y openssl
RUN apt-get install -y libeigen3-dev
RUN apt-get install -y m4

RUN cd tmp \
    && wget https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz \
    && tar -zvxf boost_1_80_0.tar.gz \
    && cd boost_1_80_0 \
    && ./bootstrap.sh \
    && ./b2 \
    && ./b2 install

RUN cd tmp \
    && git clone https://github.com/fmtlib/fmt.git \
    && cd fmt \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make -j12 \
    && make install

RUN cd tmp \
    && wget https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz \
    && tar -xf gmp-6.2.1.tar.xz \
    && cd gmp-6.2.1 \
    && ./configure --prefix=/usr  --enable-cxx \
    && make -j12 \
    && make check \
    && make install

ENV LD_LIBRARY_PATH=/usr/local/gmp/lib:$LD_LIBRARY_PATH

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 50

WORKDIR /root/PPPU
COPY . /root/PPPU

RUN mkdir build \
    && cd build \
    && cmake .. \
    && make -j12
