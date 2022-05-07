#!/bin/bash

module=scullc0

if [ ! -z "$(ls /dev | grep $module)" ]; then
	sudo dmesg -C
	sudo dmesg -D
	make
	sudo dmesg -E
	sudo ./user_cpy
	sudo dmesg | tee test_cpy.out
fi

