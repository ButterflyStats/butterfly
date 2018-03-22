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
	git

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

COPY . /butterfly

WORKDIR /butterfly

RUN git submodule init && \
	git submodule update && \
	cd build && \
	cmake -DWITH_EXAMPLES=1 -DWITH_TOOLS=1 .. && \
	make -j6 && \
	make install
