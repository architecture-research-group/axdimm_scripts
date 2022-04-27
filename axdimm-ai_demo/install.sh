#!/bin/bash

uninstall=
make
[ -z "$(sudo lsmod | grep rw)" ] && sudo insmod rw_test.ko

[ ! -z "$uninstall" ] && sudo rmmod rw_test.ko
