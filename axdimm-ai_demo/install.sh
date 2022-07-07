#!/bin/bash

module=rw_test

if [ -z "$(sudo lsmod | grep $module)" ]; then
	make -s
	sudo insmod $module.ko $* > /dev/null
	[ "$?" != "0" ] && echo "[Fatal]: module could not be loaded -- check if $module exists" && exit -1
	sudo dmesg | tail -n 5 | tee install.out
	#sudo cp -f rw_test.ko /lib/modules/`uname -r` #add symlink so modprobe can find
else 
	echo "already installed... uninstalling..."
	make -s clean
	sudo rmmod -f $module.ko &> /dev/null
	[ "$?" != "0" ] && echo "[Fatal]: module improperly loaded/unloaded -- reboot to uninstall"
	sudo dmesg | tail -n 5 | tee uninstall.out
fi

