#!/bin/sh

if [ "$1" = "" ]; then
	exit -1;
fi

if echo $1 | grep "^[^/~]" > /dev/null; then
	echo `pwd`/$1
else
	echo $1
fi
