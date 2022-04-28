#!/bin/bash

module=rw_test

if [ -z "$(sudo lsmod | grep $module)" ]; then
	sudo dmesg -C
	sudo dmesg -D
	make
	sudo dmesg -E
	sudo insmod rw_test.ko $*
	sudo dmesg | tee install.out
else 
	sudo dmesg -C
	sudo dmesg -D
	make clean
	sudo dmesg -E
	sudo rmmod rw_test.ko
	sudo dmesg | tee uninstall.out
fi

