#!/bin/sh
module="rw_test"
device="emul_mem"

# invoke rmmod with all arguments we got
if [ ! -z "$(sudo lsmod | grep $module)" ]; then
	sudo dmesg -C
	sudo dmesg -D
	sudo dmesg -E
	sudo rmmod $module $* || exit 1
	sudo dmesg -D
	sudo dmesg | tee install.out
	sudo dmesg -E
fi

# Remove stale nodes

echo "removing device $device"
sudo rm -f /dev/${device}  
