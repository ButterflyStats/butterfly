FROM ubuntu:artful

RUN apt-get update && apt-get install -y \
	gcc-5 \
	build-essential \
	unzip \
	autoconf \
	m4 \
	libtool \
	cmake \
	libsnappy-dev \
	wget \ 
	git \
	emscripten

RUN mkdir /protobuf
WORKDIR /protobuf 

RUN wget https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz && \
	tar xzf protobuf-2.6.1.tar.gz && \
	cd protobuf-2.6.1 && \
	./configure && \
	make && \
	make check && \
	make install && \
 	ldconfig

RUN mkdir /snappy
WORKDIR /snappy 

RUN wget https://github.com/google/snappy/archive/1.1.7.tar.gz && \
	tar xzf 1.1.7.tar.gz && \
	cd snappy-1.1.7 && \
	mkdir build && \
	cd build && \
	cmake ../ && \
	make && \ 
	make install
 	
COPY . /butterfly

WORKDIR /butterfly

RUN git submodule init && \
	git submodule update 

RUN cd build && \
	CC=emcc CXX=em++ cmake -I/usr/local/include .. -DWITH_JAVASCRIPT_BINDINGS=1  && \
	make -j6 
