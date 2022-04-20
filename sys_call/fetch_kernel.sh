#!/bin/bash

registered=1
compile=1
inserted=0
configured=1

[ ! -f "linux-5.8.1.tar.xz" ] && wget -P . https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.8.1.tar.xz

[ ! -d "linux-5.8.1" ] && tar -xvf linux-5.8.1.tar.xz

[ -z "$inserted" ] && cp -r axdimm_sys linux-5.8.1

if [ -z "$registered" ]; then
	sed -i '/#endif/i asmlinkage long mem_cpy_axdimm(void);\nasmlinkage long mem_read_axdimm(void);' linux-5.8.1/include/linux/syscalls.h
	sed -i '/439 common .*/a 440 common  mem_cpy_axdimm  sys_mem_cpy_axdimm\n441 common  mem_read_axdimm  sys_mem_read_axdimm\n' linux-5.8.1/arch/x86/entry/syscalls/syscall_64.tbl
	sed -i '1073s/$/ axdimm_sys\//' linux-5.8.1/Makefile
fi

#check for utils : command -v
if [ ! -z "$compile" ]; then
	cd linux-5.8.1	
	[ -z "$configured" ] && make defconfig
	make -j $(( $(nproc) - 2))
fi
