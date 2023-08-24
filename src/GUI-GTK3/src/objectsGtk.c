
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
  UI->x_circle = mapToRange(car->mpu.GYRO_X, 35000, 35600, 30, 35);
  UI->y_circle = mapToRange(car->mpu.GYRO_Y, 35000, 35600, 30, 35);
  gtk_widget_queue_draw(UI->DrawingAreaCenterCircle);
  return TRUE;
}

gboolean move_triangle(gpointer data){
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  Device *car = CAR_APP(app)->priv->device;
  ObjectsUI *UI = car_app_get_gui(app);
  UI->x_triangle=mapToRange(car->mpu.GYRO_Z,35000,35600,30,35);
  gtk_widget_queue_draw(UI->DrawingAreaTriangle);
  return TRUE;
}
/**
 * @brief
 */
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);

  if (UI->imageCenterCircle != NULL) {
    g_printerr("x=%f y=%f\n", UI->x_circle, UI->y_circle);
    gdk_cairo_set_source_pixbuf(cr, UI->imageCenterCircle, UI->x_circle, UI->y_circle);
    cairo_paint(cr);
  }
  return TRUE;
}

gboolean on_draw_triangle(GtkWidget *widget,cairo_t *cr, gpointer data){
  CARApp *app =G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI =car_app_get_gui(app);
  if (UI->imageTriangle != NULL){
    g_printerr("Triangle x=%f Triangle y=%f\n",UI->x_triangle,UI->y_triangle);
    gdk_cairo_set_source_pixbuf(cr,UI->imageTriangle,UI->x_triangle,UI->y_triangle);
    cairo_paint(cr);
  }else{
    g_printerr("error in triangle\n");
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
  // Windows
  obj->Window = GTK_WIDGET(gtk_builder_get_object(constructor, "Window"));
  obj->WindowTest = GTK_WIDGET(gtk_builder_get_object(constructor,"WindowTest"));
  // ************************************************

  // Text that can be updated
  obj->DeviceStatusText = GTK_WIDGET(gtk_builder_get_object(constructor,"DeviceStatusText"));
  obj->DriverVersionText= GTK_WIDGET(gtk_builder_get_object(constructor,"DriverVersionText"));
  obj->AutomaticTestCountdownText=GTK_WIDGET(gtk_builder_get_object(constructor,"AutomaticTestCountdownText"));
  obj->DeviceStatusMainWindowText=GTK_WIDGET(gtk_builder_get_object(constructor,"DeviceStatusMainWindowText"));
  obj->GUIVersionText=GTK_WIDGET(gtk_builder_get_object(constructor,"GUIVersionText"));
  // ************************************************

  // Buttons
  obj-> StartTestButton=GTK_WIDGET(gtk_builder_get_object(constructor,"StartTestButton"));
  obj-> SaveAndStartAppButton=GTK_WIDGET(gtk_builder_get_object(constructor,"SaveAndStartAppButton"));
  obj-> LoadMpuConfigButton=GTK_WIDGET(gtk_builder_get_object(constructor,"LoadMpuConfigButton"));
  obj-> ExitTestButton=GTK_WIDGET(gtk_builder_get_object(constructor,"ExitTestButtonl"));
  obj-> TestDeviceButton=GTK_WIDGET(gtk_builder_get_object(constructor,"TestDeviceButton"));
  //**************************************************

  // Progress Bars
  obj->ProgressBarAX = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAX"));
  obj->ProgressBarAY = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAY"));
  obj->ProgressBarAZ = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAZ"));
  obj->AnalysisProgressBar = GTK_WIDGET(gtk_builder_get_object(constructor,"AnalysisProgressBar"));
  // ************************************************

  // Objects related to the movement of the Gyroscope 
  obj->RangeCircle = GTK_WIDGET(gtk_builder_get_object(constructor, "RangeCircle"));
  obj->DrawingAreaCenterCircle = GTK_WIDGET(gtk_builder_get_object(constructor, "CenterCircle"));
  obj->DrawingAreaTriangle=GTK_WIDGET(gtk_builder_get_object(constructor,"Triangle"));
  obj->BarZaxis=GTK_WIDGET(gtk_builder_get_object(constructor,"Bar"));
  gtk_image_set_from_file(GTK_IMAGE(obj->BarZaxis),"../src_images/Rectangle.png");
  gtk_image_set_from_file(GTK_IMAGE(obj->RangeCircle), "../src_images/RG.png");
  obj->imageCenterCircle = gdk_pixbuf_new_from_file("../src_images/CC.png", NULL);
  obj->imageTriangle=gdk_pixbuf_new_from_file("../src_images/Triangle.png",NULL);
  // ************************************************

  // X & Y coordinates for the Center circle and a bar represent the Z axis movement
  obj->x_circle = xOffset;  
  obj->y_circle = yOffset;
  obj->x_triangle=180;
  obj->y_triangle=0;
  // ************************************************
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
  g_signal_connect(obj->DrawingAreaTriangle,"draw",G_CALLBACK(on_draw_triangle),app);
}
