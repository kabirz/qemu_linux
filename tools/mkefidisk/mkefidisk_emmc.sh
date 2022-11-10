#!/bin/sh

if [ "$2" = "/dev/sda" ] ; then
	echo "Warning : Do not format system partition !!!"
	exit 0;
else
	echo "intel@123" |sudo -S dd if=$1 of=$2 && sync
fi

