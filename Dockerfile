FROM ubuntu:20.04

ENV TZ=Asia/Shanghai \
    DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y apt-file software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test

RUN apt-get update \
    && apt-get upgrade \
    && apt-get install -y build-essential gcc-13 g++-13 python-dev autotools-dev cmake wget git m4 libicu-dev libbz2-dev libboost-all-dev libssl-dev openssl libeigen3-dev

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
