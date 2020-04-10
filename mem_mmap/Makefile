obj-m += wmem.o

memDiag-objs += wmem.o 

KERNEL_BASE ?= /lib/modules/$(shell uname -r)/build

DRIVER_BASE ?= $(shell pwd)

all:	clean
	$(MAKE) -C ${KERNEL_BASE} M=$(DRIVER_BASE) KERNEL_SRC=${KERNEL_BASE} 


clean:
	$(MAKE) -C ${KERNEL_BASE} M=$(DRIVER_BASE) KERNEL_SRC=${KERNEL_BASE} clean
