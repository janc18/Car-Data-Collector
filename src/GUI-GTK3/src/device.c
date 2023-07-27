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
#include <linux/hidraw.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/**
 * @brief Name of the device to search
 */
const char mpu[] = "mpu6050";

/**
 * @brief Device path
 */
const char DevicePath[] = "/dev/mpu6050";

struct udev_source *source;

void updateAxisBar(ObjectsUI *UI, guint8 number, guint16 value);

/**
 * @brief Search for an mpu6050 device.
 *
 * Look on the next path
 * - /dev/mpu6050
 *
 * And compare the name's file descriptor with mpu6050
 *
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
    close(fd);
    return 1;
  } else {
    close(fd);
    return -1;
  }
}

static gboolean CaptureEvent(gpointer data) {

  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *mpu = CAR_APP(app)->priv->device;
  struct js_event js;                           // create a struct to save all the events
  open(mpu->path, O_RDWR | O_NONBLOCK);         // open the path to read on nonblocking mode
  ssize_t len = read(mpu->fd, &js, sizeof(js)); // read the data from the device
  if (len < 0) {
    // detach device from the threadand update the visual state
    g_printerr("Error\n");
    mpu->found = FALSE;
    g_source_destroy((GSource *)source);
    // gtk_label_set_text(GTK_LABEL(UI->text_status), "Desconectado");
    // gtk_image_set_from_file(GTK_IMAGE(UI->visual_status), "../src_images/red.png");
    return TRUE;
  }

  if (len == sizeof(js)) {
    if (js.type & JS_EVENT_AXIS) {
      updateAxisBar(UI, js.number, js.value);
      g_printerr("Axis: %d, Value: %d\n", js.number, js.value);
    } else if (js.type & JS_EVENT_BUTTON) {

      g_printerr("Button: %d, Value: %d\n", js.number, js.value);
    }
  }
  return TRUE;
}
/**
 *@brief Print on terminal Device's information
 *
 *Like:
 * - File Descriptor
 * - Device Version
 * - Number of Axis
 * - Number of Buttons
 * - Path on /dev/
 *
 *@param Device* Struct with all the Device data
 */

void showDevInfo(Device *mpu) {

  g_printerr("\n-----------------\n");
  g_printerr("File Descriptor %d \n", mpu->fd);
  g_printerr("Device Version %d \n", mpu->version);
  g_printerr("-------------------\n");
}

struct udev_source {
  GSource base;
  gpointer tag;
};

static gboolean udev_source_prepare(G_GNUC_UNUSED GSource *source, gint *timeout) {
  *timeout = -1;
  return FALSE;
}

static gboolean udev_source_check(GSource *source) {
  struct udev_source *usrc = (struct udev_source *)source;
  return (g_source_query_unix_fd(source, usrc->tag) > 0);
}

static gboolean udev_source_dispatch(GSource *source, GSourceFunc callback, gpointer user_data) {
  struct udev_source *usrc = (struct udev_source *)source;
  GIOCondition revents = g_source_query_unix_fd(source, usrc->tag);

  if (revents & G_IO_IN | G_IO_HUP) {
    if (callback == NULL)
      return FALSE;
    return callback(user_data);
  }

  return TRUE;
}

GSourceFuncs udev_source_funcs = {
    .prepare = udev_source_prepare,
    .check = udev_source_check,
    .dispatch = udev_source_dispatch,
};

void configGSource(Device *mpu, CARApp *app) {

  source = (struct udev_source *)g_source_new(&udev_source_funcs, sizeof(*source));
  g_source_set_callback(&source->base, CaptureEvent, app, NULL); /* destroy_notify */
  source->tag = g_source_add_unix_fd(&source->base, mpu->fd, G_IO_IN | G_IO_HUP);
  g_source_attach(&source->base, g_main_context_default());
  g_source_unref(&source->base);
}

/**
 *@brief Update the steering wheel rotation
 *@param *ObjectsUI Struct with all the UI's components
 *@param guint16 Value of rotation
 *@todo Add implicity formula
 */
void updateSteeringWheel(ObjectsUI *UI, guint16 value) {
  // UI->rotation = (value / 32000.0) * 8;
  // gtk_widget_queue_draw(UI->swa);
}
/**
 *@brief Update level bar ,corresponded to one axis
 *@param *ObjectsUI Struct with all the UI's components
 *@param guint8 Axis'number for example: number=1 correspond to the brake
 *@param guint16 Value of axis
 */
void updateAxisBar(ObjectsUI *UI, guint8 number, guint16 value) {
  switch (number) {
  case 0: {
    // gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(UI->barra_freno), value / 32000.0);
    break;
  }
  case 3: {
    // gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(UI->barra_clutch), value / 32000.0);
    break;
  }
  case 2: {
    // gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(UI->barra_acelerador), value / 32000.0);
    break;
  }
  case 1: {
    updateSteeringWheel(UI, value);

    break;
  }
  }
}
