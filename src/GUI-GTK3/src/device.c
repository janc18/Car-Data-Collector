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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief Name of the device to search
 */
const char mpu[] = "mpu6050";

/**
 * @brief Device path
 */
const char DevicePath[] = "/dev/mpu6050";

unsigned int ioctlCmd[6] = {GX, GY, GZ, AX, AY, AZ};

/**
 * @brief Search for an mpu6050 device.
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
    //close(fd);
    mpu->found = true;
    mpu->fd = fd;
    g_printerr("%d",mpu->fd);
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
 * - Path
 * - Device found
 *@param Device* Struct with all the Device data
 */

void showDevInfo(Device *mpu) {

  g_printerr("\n-----------------\n");
  g_printerr("File Descriptor %d \n", mpu->fd);
  g_printerr("Device Version %d \n", mpu->version);
  g_printerr(mpu->found ? "Device found\n" : "Device not found\n");
  g_printerr("%d\n", mpu->mpu.GYRO_X);
  g_printerr("%d\n", mpu->mpu.GYRO_Y);
  g_printerr("%d\n", mpu->mpu.GYRO_Z);
  g_printerr("-------------------\n");
}
/**
 *@brief Update level bar ,corresponded to one axis
 *@param *ObjectsUI Struct with all the UI's components
 *@param guint8 Axis'number for example: number=1 correspond to the brake
 *@param guint16 Value of axis
 */

void SetDataToBarAndText(GtkWidget *LevelBar, int32_t MpuValue, GtkWidget *LabelValue) {
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(LevelBar), MpuValue / 65535.0);
  char Buffer[15];
  snprintf(Buffer, sizeof(Buffer), "%d", MpuValue);
  gtk_label_set_text(GTK_LABEL(LabelValue), Buffer);
}
void GetDataFromDriverIOCTL(Device *car) {
  car->mpu.GYRO_X=0;
  car->mpu.GYRO_Z=0;
  car->mpu.GYRO_Y=0;
  car->mpu.ACCEL_X=0;
  car->mpu.ACCEL_Y=0;
  car->mpu.ACCEL_Z=0;
  ioctl(car->fd, GX, (int32_t*)&car->mpu.GYRO_X);
  usleep(18000);
  ioctl(car->fd, GY, (int32_t*)&car->mpu.GYRO_Y);
  usleep(18000);
  ioctl(car->fd, GZ, (int32_t*)&car->mpu.GYRO_Z);
  usleep(18000);
}

gboolean UpdateVisualData(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;
  GetDataFromDriverIOCTL(car);
#ifdef DEBUG
  showDevInfo(car);
  g_printerr("Triggered function \"UpdateVisualData\"\n");
#endif
  SetDataToBarAndText(UI->YawLevelBar, car->mpu.GYRO_X, UI->YawText);
  SetDataToBarAndText(UI->RollLevelBar, car->mpu.GYRO_Y, UI->RollText);
  SetDataToBarAndText(UI->PitchLevelBar, car->mpu.GYRO_Z, UI->PitchText);
  return TRUE;
}
