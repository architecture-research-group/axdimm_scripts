#!/bin/sh
module="rw_test"
device="emul_mem"
mode="664"

# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
if [ ! -f "./$module.ko" ]; then
	sudo dmesg -C
	sudo dmesg -D
	make
fi

if [ -z "$(sudo lsmod | grep $module)" ]; then
	sudo dmesg -C
	sudo dmesg -E
	sudo /sbin/insmod ./$module.ko $* || exit 1
	sudo dmesg | tee install.out
	# remove stale nodes
	sudo rm -f /dev/${device}

	major=`cat /proc/devices | awk "\\$2==\"$device\" {print \\$1}" | tail -n 1`

	sudo mknod /dev/${device} c $major 0

	# give appropriate group/permissions, and change the group.
	# Not all distributions have staff, some have "wheel" instead.
	group="staff"
	grep -q '^staff:' /etc/group || group="wheel"

	sudo chgrp $group /dev/${device}
	sudo chmod $mode  /dev/${device}
fi
