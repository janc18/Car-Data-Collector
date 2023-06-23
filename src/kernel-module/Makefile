
OUTPUT_DIR := output
obj-m += car_data_collector.o

KDIR := ../arm-kernel/linux/

all:
	mkdir -p $(OUTPUT_DIR)
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
	mv -f *.order *.symvers *.ko *.mod.c *.mod.o *.mod *.o $(OUTPUT_DIR) 2>/dev/null ||:
 
clean:
#	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean
	rm -rf $(OUTPUT_DIR)
