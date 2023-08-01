/*
 * Copyright (C) 2023 OpenHF
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include "device.h"
#include "app.h"
#include "glibconfig.h"
#include <fcntl.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief Const float to smooth the data
 */
const float ALPHA = 0.3;

/**
 * @brief Name of the device to search
 */
const char mpu[] = "mpu6050";

/**
 * @brief Device path
 */
const char DevicePath[] = "/dev/mpu6050";

/**
 * @brief MPU6050 Array with IOCTL Commands to get the raw values
 */
unsigned int ioctlCmd[6] = {GX, GY, GZ, AX, AY, AZ};

/**
 * @brief Search for an mpu6047 device.
 *
 * Look on the next path
 * - /dev/mpu6050
 * And compare the name's file descriptor with mpu6050
 * @param *Device Struct with all the Device data
 * @return
 * - 1 Device found
 * - -1 No device found
 */
int searchMpu6050Device(Device *mpu) {
  memset(mpu->path, 0, sizeof(mpu->path));
  char buffer[256];
  char path[50];
  int fd, i;
  g_printerr("Searching for Device\n");
  fd = open(DevicePath, O_RDWR);
  if (fd > 0) {
    mpu->found = true;
    mpu->fd = fd;
    return 1;
  } else {
    mpu->found = false;
    close(fd);
    return -1;
  }
}

/**
 *@brief Print on terminal Device's information
 *
 *Like:
 * - File Descriptor
 * - Device Version
 * - Device found
 * - Raw values from the mpu(Gyroscope and Accelerometer)
 *@param Device* Struct with all the Device data
 */
void showDevInfo(Device *car) {
  int32_t *pMpuValues[6] = {&car->mpu.GYRO_X, &car->mpu.GYRO_Y, &car->mpu.GYRO_Z, &car->mpu.ACCEL_X, &car->mpu.ACCEL_Y, &car->mpu.ACCEL_Z};
  g_printerr("\n-----------------\n");
  g_printerr("File Descriptor %d \n", car->fd);
  g_printerr("Device Version %d \n", car->version);
  g_printerr(car->found ? "Device found\n" : "Device not found\n");
  for (int i = 0; i < 6; i++) {
    g_printerr("%d\n", *(pMpuValues[i]));
  }
  g_printerr("-------------------\n");
}

/**
 *@brief Set data to Levels Bars and Labels
 *@param *GtkWidget Level Bar Widget
 *@param int32_t Raw value from the mpu
 *@param *GtkWidget Label Widget
 */
void SetDataToBarAndText(GtkWidget *LevelBar, int32_t MpuValue, GtkWidget *LabelValue) {
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(LevelBar), MpuValue / 65535.0);
  char Buffer[15];
  snprintf(Buffer, sizeof(Buffer), "%d", MpuValue);
  gtk_label_set_text(GTK_LABEL(LabelValue), Buffer);
}

/**
 * @brief Get data from the Driver with IOCTL commands and save in the car struct
 * @param *Device car struct
 */
void GetDataFromDriverIOCTL(Device *car) {
  int32_t *pMpuValues[6] = {&car->mpu.GYRO_X, &car->mpu.GYRO_Y, &car->mpu.GYRO_Z, &car->mpu.ACCEL_X, &car->mpu.ACCEL_Y, &car->mpu.ACCEL_Z};
  for (int i = 0; i < 6; i++) {
    *(pMpuValues[i]) = 0;
  }
  for (int i = 0; i < 6; i++) {
    ioctl(car->fd, ioctlCmd[i], (int32_t *)pMpuValues[i]);
    usleep(18000);
  }
  for (int i = 0; i < 6; i++) {
    *(pMpuValues[i]) = exponential_moving_average_filter(*(pMpuValues[i]));
  }
}

/**
 * @brief Draw the new values to level bars and labels
 * @param gpointer with CARApp data
 * @return TRUE
 */
gboolean UpdateVisualData(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;
  GetDataFromDriverIOCTL(car);
  int32_t *pMpuValues[6] = {&car->mpu.GYRO_X, &car->mpu.GYRO_Y, &car->mpu.GYRO_Z, &car->mpu.ACCEL_X, &car->mpu.ACCEL_Y, &car->mpu.ACCEL_Z};
  GtkWidget **Labels[6] = {&UI->GXText, &UI->GYText, &UI->GZText, &UI->AccelXText, &UI->AccelYText, &UI->AccelZText};
  GtkWidget **LevelBar[6] = {&UI->GXLevelBar, &UI->GYLevelBar, &UI->GZLevelBar, &UI->AccelXLevelBar, &UI->AccelYLevelBar, &UI->AccelZLevelBar};
#ifdef DEBUG
  showDevInfo(car);
  g_printerr("Triggered function \"UpdateVisualData\"\n");
#endif
  for (int i = 0; i < 6; i++) {
    SetDataToBarAndText(*(LevelBar[i]), *(pMpuValues[i]), *(Labels[i]));
  }
  return TRUE;
}

/**
 * @brief Filter to smooth data
 * @param int data to be smoothed
 * @return int value smoothed
 */
int exponential_moving_average_filter(int new_data) {
  static float smoothed_data = 0;
  smoothed_data = (1 - ALPHA) * smoothed_data + ALPHA * new_data;
  return (int)round(smoothed_data);
}
