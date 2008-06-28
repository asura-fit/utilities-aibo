#!/bin/sh


echo "Confirm you are ROOT!!"
echo "Setting up SysV message queue limits in linux kernel"

echo 256 > /proc/sys/kernel/msgmni
