#!/bin/bash

if [ "$1" == "" ]; then
	echo "Usage: $0 <minidump file>"
	exit 1
fi

./minidump-2-core $1 > $1.mcore
