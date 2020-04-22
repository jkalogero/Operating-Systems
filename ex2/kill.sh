#!/bin/bash

echo "arg 1: $1"
echo "arg 2: $2"


if [ "$1" == "1" ]
then
kill -SIGUSR1 "$2"
elif [ "$1" == "2" ]
then
kill -SIGUSR2 "$2"
elif [ "$1" == "t" ]
then
kill -SIGTERM "$2"
fi