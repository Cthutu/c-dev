#!/usr/bin/env bash

./build.sh
if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi

./_bin/hello.exe
