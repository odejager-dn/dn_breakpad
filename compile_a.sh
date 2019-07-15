#!/bin/bash
rm -f minidump.o
g++ -ggdb -fPIC -c minidump.c -I . -lbreakpad_client -L . -lpthread
[ -e minidump.o ] && ar rcs libminidump.a minidump.o || echo failed

