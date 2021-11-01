#!/usr/bin/sh

if ! watchexec --version &> /dev/null
then
    watchexec -Nc -e c,h "make config=release && ./tool/bin/run-tests" 
else
    echo "[error] watchexec binary not found, install it at https://github.com/watchexec/watchexec"
fi
