#!/bin/bash

module=rw_test

if [ -z "$(sudo lsmod | grep $module)" ]; then
	sudo insmod $module.ko $* > /dev/null
	[ "$?" != "0" ] && echo "[Fatal]: module could not be loaded -- check if $module exists" && exit -1
	echo "------------------Last 10 Log Messages ------------------"
	sudo dmesg | tail -n 10 | tee install.out
else
	echo "already installed..."
fi

