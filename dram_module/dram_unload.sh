#!/bin/sh
module="rw_test"
device="emul_mem"

# invoke rmmod with all arguments we got
sudo rmmod $module $* || exit 1

# Remove stale nodes

sudo rm -f /dev/${device}  
