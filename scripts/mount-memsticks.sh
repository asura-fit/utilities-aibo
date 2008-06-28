#!/bin/sh

echo "Confirm you are root."

mkdir /g /e /h
umount /g
umount /e
umount /h

smbmount //131.206.93.110/G /g -o username=asura,password=kerorin,fmask=666,dmask=777
smbmount //131.206.93.53/E /e -o username=administrator,password=kerorin,fmask=666,dmask=777
smbmount //131.206.93.65/E /h -o username=administrator,password=kerorin,fmask=666,dmask=777

