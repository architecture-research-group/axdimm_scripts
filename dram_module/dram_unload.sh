#!/bin/sh
module="rw_test"
device="emul_mem"

# invoke rmmod with all arguments we got
rmmod $module $* || exit 1

# Remove stale nodes

rm -f /dev/${device}  
