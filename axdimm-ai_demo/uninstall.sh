#!/bin/bash
[ -z "$(sudo lsmod | grep rw_test)" ] && exit
dmesg -C
dmesg -E

[ -z "$(sudo lsmod | grep rw_test)" ] && make clean && sudo rmmod rw_test.ko

insmod rw_test.ko $*
dmesg -D
dmesg | tee install.out

