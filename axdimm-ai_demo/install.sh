#!/bin/bash

module=rw_test

if [ -z "$(sudo lsmod | grep $module)" ]; then
	sudo dmesg -C
	sudo dmesg -D
	make -s
	sudo dmesg -E
	sudo insmod rw_test.ko $* > /dev/null
	[ "$?" != "0" ] && echo "[Fatal]: module could not be loaded -- check if $module exists" && exit -1
	sudo dmesg | tee install.out
	#sudo cp -f rw_test.ko /lib/modules/`uname -r` #add symlink so modprobe can find
else 
	echo "already installed... uninstalling..."
	sudo dmesg -C
	sudo dmesg -D
	make -s clean
	sudo dmesg -E
	sudo rmmod -f rw_test.ko &> /dev/null
	[ "$?" != "0" ] && echo "[Fatal]: module improperly loaded/unloaded -- reboot to uninstall"
	sudo dmesg | tee uninstall.out
fi

