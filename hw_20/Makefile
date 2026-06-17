obj-m := test3.o

KERNEL_DIR := $(HOME)/WSL2-Linux-Kernel

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean


.PHONY: all clean