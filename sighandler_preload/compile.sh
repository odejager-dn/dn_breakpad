#!/bin/bash

rm -f handler.o handler.so segfaulty
gcc -ggdb -fPIC -c handler.c && ld -shared -shared handler.o -ldl -L . -lminidump -o handler.so
if [ -e handler.so ]; then
    echo building segfaulty
    gcc segfaulty.c -o segfaulty && LD_PRELOAD=$PWD/handler.so ./segfaulty
else
    echo failed
fi
