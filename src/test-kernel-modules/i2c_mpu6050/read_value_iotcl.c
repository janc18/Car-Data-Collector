/**
 *  \file       test_app.c
 *
 *  \details    Userspace application to test the Device driver
 *
 *  \author     EmbeTronicX
 *
 *  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
 *  \Tested with Linux raspberrypi 5.15.84+
 *******************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
// ioctl
#define GX _IOR('a', 'a', int16_t *)
#define GY _IOR('a', 'b', int16_t *)
#define GZ _IOR('a', 'c', int16_t *)
#define AX _IOR('a', 'd', int16_t *)
#define AY _IOR('a', 'e', int16_t *)
#define AZ _IOR('a', 'f', int16_t *)

char RegistersNames[][6] = {"GX", "GY", "GZ", "AX", "AY", "AZ"};

unsigned int ks[6] = {GX, GY, GZ, AX, AY, AZ};

int fd;
int main() {
  int32_t value, number;
  fd = open("/dev/mpu6050", O_RDWR);
  printf("\nOpening Driver\n");
  if (fd < 0) {
    printf("Cannot open device file...\n");
    return 0;
  }

  for (int i = 0; i < 6; i++) {
    ioctl(fd, ks[i], (int32_t *)&value);
    printf("Value of %s is %d\n", RegistersNames[i], value);
    usleep(18000);
  }

  printf("Closing Driver\n");
  close(fd);
}
