obj-m += gps.o

all: module dt
	echo Bulded Device Tree Overlay and kernel module

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

dt :serdev_overlay.dts
	dtc -@ -I dts -O dtb -o serdev_overlay.dtbo serdev_overlay.dts

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


