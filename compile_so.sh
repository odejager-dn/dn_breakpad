#!/bin/bash

g++ -ggdb -fPIC -shared -o libminidump.so minidump.cpp -I . -lbreakpad_client -L . -lpthread

