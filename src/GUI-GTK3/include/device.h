#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <gtk/gtk.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#define GX _IOR('a', 'a', int16_t *)
#define GY _IOR('a', 'b', int16_t *)
#define GZ _IOR('a', 'c', int16_t *)
#define AX _IOR('a', 'd', int16_t *)
#define AY _IOR('a', 'e', int16_t *)
#define AZ _IOR('a', 'f', int16_t *)

typedef struct RawMpuValues {
  int32_t ACCEL_X;
  int32_t ACCEL_Y;
  int32_t ACCEL_Z;
  int32_t GYRO_X;
  int32_t GYRO_Y;
  int32_t GYRO_Z;
} RawMpuValues;

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
  RawMpuValues mpu;
} Device;

int searchMpu6050Device(Device *mpu);
void showDevInfo(Device *mpu);
gboolean UpdateVisualData(gpointer data);
int exponential_moving_average_filter(int new_data);
#endif
