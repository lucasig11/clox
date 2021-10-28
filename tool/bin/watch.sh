#!/usr/bin/sh

chap=$1

if ["$chap" == ""]; then
    chap="chap24_calls"
fi

echo "Testing $chap"

if ! watchexec --version &> /dev/null
then
    watchexec -Nc -e c,h "make config=release && dart tool/bin/test.dart $chap --interpreter bin/release/clox 2> /dev/null" 
else
    echo "[error] watchexec binary not found."
fi