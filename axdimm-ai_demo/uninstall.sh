#!/bin/bash

module=rw_test

make -s clean
sudo rmmod -f $module.ko &> /dev/null
[ "$?" != "0" ] && echo "[Fatal]: module improperly loaded/unloaded -- reboot to uninstall"
echo "------------------Last 10 Log Messages ------------------"
sudo dmesg | tail -n 10 | tee uninstall.out
