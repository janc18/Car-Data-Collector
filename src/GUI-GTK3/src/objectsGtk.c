
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
#include "device.h"
#include "drawing.h"
#include "windows.h"
#include <cairo.h>
#include <gtk/gtk.h>
#include <sys/ioctl.h>

const char pathGladeFile[] = "../gladeFiles/GUI_car_1.glade";

const gchar GUI_VERSION[] = "1.0";

const gdouble xOffset = 155;
const gdouble yOffset = 195;

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
  obj->WindowTest = GTK_WIDGET(gtk_builder_get_object(constructor, "WindowTest"));
  obj->RootDialog = GTK_WIDGET(gtk_builder_get_object(constructor, "RootDialog"));
  // ************************************************

  // Text that can be updated
  obj->DeviceStatusText = GTK_WIDGET(gtk_builder_get_object(constructor, "DeviceStatusText"));
  obj->DriverVersionText = GTK_WIDGET(gtk_builder_get_object(constructor, "DriverVersionText"));
  obj->AutomaticTestCountdownText = GTK_WIDGET(gtk_builder_get_object(constructor, "AutomaticTestCountdownText"));
  obj->DeviceStatusMainWindowText = GTK_WIDGET(gtk_builder_get_object(constructor, "DeviceStatusMainWindowText"));
  obj->GUIVersionText = GTK_WIDGET(gtk_builder_get_object(constructor, "GUIVersionText"));
  // ************************************************

  // Buttons
  obj->StartTestButton = GTK_WIDGET(gtk_builder_get_object(constructor, "StartTestButton"));
  obj->SaveAndStartAppButton = GTK_WIDGET(gtk_builder_get_object(constructor, "SaveAndStartAppButton"));
  obj->LoadMpuConfigButton = GTK_WIDGET(gtk_builder_get_object(constructor, "LoadMpuConfigButton"));
  obj->ExitTestButton = GTK_WIDGET(gtk_builder_get_object(constructor, "ExitTestButton"));
  obj->TestDeviceButton = GTK_WIDGET(gtk_builder_get_object(constructor, "TestDeviceButton"));
  obj->AcceptDialogButton = GTK_WIDGET(gtk_builder_get_object(constructor, "AcceptDialogButton"));
  obj->ExitDialogButton = GTK_WIDGET(gtk_builder_get_object(constructor, "ExitDialogButton"));
  //**************************************************

  // Progress Bars
  obj->ProgressBarAX = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAX"));
  obj->ProgressBarAY = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAY"));
  obj->ProgressBarAZ = GTK_WIDGET(gtk_builder_get_object(constructor, "ProgressBarAZ"));
  obj->AnalysisProgressBar = GTK_WIDGET(gtk_builder_get_object(constructor, "AnalysisProgressBar"));
  // ************************************************

  // Objects related to the movement of the Gyroscope
  obj->RangeCircle = GTK_WIDGET(gtk_builder_get_object(constructor, "RangeCircle"));
  obj->DrawingAreaCenterCircle = GTK_WIDGET(gtk_builder_get_object(constructor, "CenterCircle"));
  obj->DrawingAreaTriangle = GTK_WIDGET(gtk_builder_get_object(constructor, "Triangle"));
  obj->BarZaxis = GTK_WIDGET(gtk_builder_get_object(constructor, "Bar"));
  gtk_image_set_from_file(GTK_IMAGE(obj->BarZaxis), "../src_images/Rectangle.png");
  gtk_image_set_from_file(GTK_IMAGE(obj->RangeCircle), "../src_images/RG.png");
  obj->imageCenterCircle = gdk_pixbuf_new_from_file("../src_images/CC.png", NULL);
  obj->imageTriangle = gdk_pixbuf_new_from_file("../src_images/Triangle.png", NULL);
  // ************************************************

  // Gui Version
  gtk_label_set_text(GTK_LABEL(obj->GUIVersionText), (gchar *)GUI_VERSION);
  // ************************************************

  // TextView
  obj->OutputStatusTest = GTK_WIDGET(gtk_builder_get_object(constructor, "OutputStatusTest"));
  // ***********************************************

  // X & Y coordinates for the Center circle and a bar represent the Z axis movement
  obj->x_circle = xOffset;
  obj->y_circle = yOffset;
  obj->x_triangle = 180;
  obj->y_triangle = 0;
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
  g_signal_connect(obj->DrawingAreaTriangle, "draw", G_CALLBACK(on_draw_triangle), app);
  g_signal_connect(obj->TestDeviceButton, "clicked", G_CALLBACK(open_test_window), app);
  g_signal_connect(obj->AcceptDialogButton, "clicked", G_CALLBACK(close_message), app);
  g_signal_connect(UI->ExitTestButton, "clicked", G_CALLBACK(close_test_window), app);
  g_signal_connect_swapped(UI->StartTestButton, "clicked", G_CALLBACK(starting_test), app);
  g_signal_connect_swapped(obj->ExitDialogButton, "clicked", G_CALLBACK(freeElements), app);
}
