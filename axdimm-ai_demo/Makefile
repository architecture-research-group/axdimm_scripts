obj-m += rw_test_2.o

ccflags-y := -O0 -mpreferred-stack-boundary=4 -mavx2 #-ftree-vectorize

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
