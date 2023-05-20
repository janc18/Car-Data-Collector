
OUTPUT_DIR := output
obj-m += i2c_mpu6050.o
KDIR := ../../../arm-kernel/5.15/linux/

EXTRA_CFLAGS += -DMODULE -D__KERNEL__ -Wno-unused-parameter


all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules 
#	mv -f *.order *.symvers *.ko *.mod.c *.mod.o *.mod *.o $(OUTPUT_DIR) 2>/dev/null ||:
 
clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean
#	rm -rf $(OUTPUT_DIR)
#	rm .**.cmd
