
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

#include "objectsGtk.h"
#include "app.h"
#include <cairo.h>
#include <gtk/gtk.h>
const char pathGladeFile[] = "../gladeFiles/GUI_car_1.glade";

const gdouble xOffset = 155;
const gdouble yOffset = 210;

/**
 * @brief Start gui
 * Load glade files
 */
void start_gui(void) {
  GError *error = NULL;
  GtkBuilder *constructor = gtk_builder_new();
  gtk_init(NULL, NULL);

  if (gtk_builder_add_from_file(constructor, pathGladeFile, &error) == 0) {
    g_printerr("Error no se encontro el archivo :%s \n", error->message);
    g_clear_error(&error);
  }

  g_set_prgname("CAR");
  g_object_unref(G_OBJECT(constructor));
  gtk_main();
}
/**
 * @brief
 */
gboolean move_image(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;
  UI->x = mapToRange(car->mpu.GYRO_X, 35000, 35600, 30, 35);
  UI->y = mapToRange(car->mpu.GYRO_Y, 35000, 35600, 30, 35);
  gtk_widget_queue_draw(UI->DrawingAreaCenterCircle);
  return TRUE;
}
/**
 * @brief
 */
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);

  if (UI->imageCenterCircle != NULL) {
    g_printerr("x=%f y=%f\n", UI->x, UI->y);
    gdk_cairo_set_source_pixbuf(cr, UI->imageCenterCircle, UI->x, UI->y);
    cairo_paint(cr);
  }

  return TRUE;
}

gdouble mapToRange(gdouble value, gdouble minInput, gdouble maxInput, gdouble minOutput, gdouble maxOutput) {
  double mappedValue = -1 + 2 * (value - minInput) / (maxInput - minInput);
  double result = minOutput + (mappedValue + 1) * (maxOutput - minOutput) / 2;
  return result;
}

/**
 * @brief Build all GUI objects
 * @param GtkApplication pointer with
 */
ObjectsUI *buildObjects(GtkApplication *app) {
  GtkBuilder *constructor = gtk_builder_new();
  gtk_builder_add_from_file(constructor, pathGladeFile, NULL);
  ObjectsUI *obj = g_malloc(sizeof(ObjectsUI));

#ifdef DEBUG
  g_printerr("UI %p \n", obj);
#endif

  obj->Window = GTK_WIDGET(gtk_builder_get_object(constructor, "Window"));

  obj->ProgressBarAX = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAX"));
  obj->ProgressBarAY = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAY"));
  obj->ProgressBarAZ = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAZ"));
  obj->RangeCircle = GTK_WIDGET(gtk_builder_get_object(constructor, "RangeCircle"));
  obj->DrawingAreaCenterCircle = GTK_WIDGET(gtk_builder_get_object(constructor, "CenterCircle"));
  gtk_image_set_from_file(GTK_IMAGE(obj->RangeCircle), "../src_images/RG.png");
  obj->imageCenterCircle = gdk_pixbuf_new_from_file("../src_images/CC.png", NULL);
  obj->x = xOffset;
  obj->y = yOffset;
  return obj;
  g_object_unref(G_OBJECT(constructor));
}
/**
 * @brief Deallocated memory
 *
 * Deallocated GUI elements and device data
 *
 * @param gpoiter that contains Device,ObjectsUI and app
 */
void freeElements(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;

#ifdef DEBUG
  g_printerr("%p UI\n", UI);
  g_printerr("%p device\n", car);
#endif
  g_free(car);
  g_free(UI);
  gtk_main_quit();
}
/**
 * @brief Connected signals
 *
 * Connect ObjectsUI with callbacks
 * @param ObjectsUI struct with all the GtkWidget elements
 * @param CARApp
 */
void signalsConnection(ObjectsUI *obj, CARApp *app) {
  ObjectsUI *UI = car_app_get_gui(app);
  g_signal_connect_swapped(obj->Window, "destroy", G_CALLBACK(freeElements), app);
  g_signal_connect(obj->DrawingAreaCenterCircle, "draw", G_CALLBACK(on_draw), app);
}
