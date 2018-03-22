About
=====

butterfly is a Dota 2 replay parser focused on Source2, built by [invokr](https://github.com/invokr). Consider it the spiritual successor of his [Alice](https://github.com/invokr/alice2) project.

Unfortunately the original author of this code has left forever, so [noxville](https://github.com/noxville) and [spheenik](https://github.com/spheenik) have taken over this marvellous example of engineering.

Building
========

Butterfly has Linux and macOS support (C++). Bindings exist for python and Javascript (using emscripten).

This should do the trick on debian-flavoured systems:

    apt-get install build-essential cmake libprotobuf-dev libsnappy-dev protobuf-compiler
    git submodule init
    git submodule update
    cd build
    cmake -DWITH_EXAMPLES=1 -DWITH_TOOLS=1 ..
    make -j6
    make install

The CMake part for building javascript libaries looks like this:

    cd build
    rm -Rf *        # Make sure the build folder is empty / doesn't contain a previous CMake build
    CC=emcc CXX=em++ cmake .. -DWITH_JAVASCRIPT_BINDINGS=1
    make -j6

This should generate butterflyjs0.js and butterflyjs1.js. Use the later for a stable JS api.

Docker
====

You can use the included Dockerfile to build a docker image of the project - and then just copy out the built binaries or use them in-place.

License
=======

Apache 2
