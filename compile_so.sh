#!/bin/bash

g++ -ggdb -fPIC -shared -o libminidump.so minidump.c -I . -lbreakpad_client -L . -lpthread

