FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
	gcc \
	build-essential \
	cmake \
	libsnappy-dev \
	libprotobuf-dev \
	protobuf-compiler

COPY . /butterfly

WORKDIR /butterfly

RUN mkdir build && \
	cd build && \
	cmake -DWITH_EXAMPLES=1 -DWITH_TOOLS=1 .. && \
	make -j && \
	make install
