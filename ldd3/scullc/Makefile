
# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y


ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULLC_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

LDDINC=$(PWD)/../include
EXTRA_CFLAGS += $(DEBFLAGS) -I$(LDDINC)

TARGET = scullc

ifneq ($(KERNELRELEASE),)

scullc-objs := main.o scull-shared/scull-async.o

obj-m	:= scullc.o

else


ifneq ("$(VM_KERNEL)", "")
  KERNELDIR ?= /lib/modules/$(VM_KERNEL)/build
else
  KERNELDIR ?= /lib/modules/$(shell uname -r)/build
endif
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif


install:
	install -d $(INSTALLDIR)
	install -c $(TARGET).o $(INSTALLDIR)

mmap: mmap.c
	gcc -g -o mmap mmap.c

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.mod modules.order *.symvers scull-shared/scull-async.o


depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
