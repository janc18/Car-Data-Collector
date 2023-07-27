#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <gtk/gtk.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
/**
 *@brief Structure to hold information about a device.
 *This structure contains information about a device,
 *including its file descriptor, path, whether it is an HID device or not,
 *whether it has been found or not, its version,
 *the number of axis it has and the number of buttons it has.
 */
typedef struct Device {
  int fd;
  char path[50];
  bool found;
  guint32 version;
  guint8 axis;
} Device;

int searchMpu6050Device(Device *mpu);
void showDevInfo(Device *mpu);

#endif
// #pragma once
