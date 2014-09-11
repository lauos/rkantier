KERNEL_SRC:=/home/gavin/Android/kernel/goldfish
PWD:=$(shell pwd)
ARCH=arm
CROSS_COMPILE=/home/gavin/Android/Android-2.3.7/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld

obj-m:= rootkit.o
all: 
	$(MAKE) -C $(KERNEL_SRC) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules
clean:
	rm *.o *.ko
