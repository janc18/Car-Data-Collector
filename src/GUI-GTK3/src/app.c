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

#include "app.h"
#include "device.h"
#include "objectsGtk.h"
G_DEFINE_TYPE_WITH_CODE(CARApp, car_app, GTK_TYPE_APPLICATION, G_ADD_PRIVATE(CARApp));

static void car_app_init(CARApp *app) {
  app->priv = car_app_get_instance_private(app);
  app->priv->most_recent_id = 1;
}

static void car_app_activate(GApplication *app) {
  Device *device = g_malloc(sizeof(Device));
  CAR_APP(app)->priv->UI = buildObjects(GTK_APPLICATION(app));
  ObjectsUI *UI = car_app_get_gui(CAR_APP(app));
  g_set_prgname("CAR");
  searchMpu6050Device(device);
  g_printerr(device->found ? "Device found\n" : "Device not found\n");
 // 
  signalsConnection(UI, CAR_APP(app));
  CAR_APP(app)->priv->device = device;
  if (device->found) {
  	guint timer_id = g_timeout_add(150, UpdateVisualData, app);
  }
  gtk_main();
}

static void car_app_startup(GApplication *app) { G_APPLICATION_CLASS(car_app_parent_class)->startup(app); }
GtkApplication *car_app_new(void) { return g_object_new(CAT_TYPE_CAR_APP, "application_id", "net.cat.car", "flags", G_APPLICATION_FLAGS_NONE, NULL); }
ObjectsUI *car_app_get_gui(CARApp *app) { return app->priv->UI; }
static void car_app_class_init(CARAppClass *class) {
  G_APPLICATION_CLASS(class)->startup = car_app_startup;
  G_APPLICATION_CLASS(class)->activate = car_app_activate;
}
