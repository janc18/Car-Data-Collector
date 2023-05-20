
OUTPUT_DIR := output
obj-m += gps_uart 

KDIR := ../arm-kernel/linux/

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
 
clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean
