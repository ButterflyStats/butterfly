FROM debian:bookworm-slim

RUN set -eux; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		gcc \
		build-essential \
		cmake \
		ninja-build \
		libsnappy-dev \
		libprotobuf-dev \
		protobuf-compiler

COPY . /butterfly

WORKDIR /butterfly

RUN cmake -B build/ -GNinja -DCMAKE_BUILD_TYPE=Release -DWITH_EXAMPLES=1 -DWITH_TOOLS=1 && \
	ninja -C build install all
