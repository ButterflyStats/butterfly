About
=====

butterfly is a Dota 2 replay parser focused on Source2. Consider it the spiritual successor of my [Alice]() project.

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

Todo
====

Actually fill this readme with some useful information.

License
=======

Apache 2
