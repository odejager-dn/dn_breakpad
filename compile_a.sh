#!/bin/bash
rm -f minidump.o
g++ -ggdb -fPIC -c minidump.cpp -I . -lpthread
[ -e minidump.o ] && ar rcs libminidump.a minidump.o libbreakpad_client.a || echo failed

