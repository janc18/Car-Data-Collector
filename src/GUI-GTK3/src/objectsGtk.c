
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
/**
 * @brief Start gui
 * Load glade files
 */
void start_gui(void) {
  GError *error = NULL;
  GtkBuilder *constructor = gtk_builder_new();
  gtk_init(NULL, NULL);

  if (gtk_builder_add_from_file(constructor, "../gladeFiles/GUI_car.glade", &error) == 0) {
    g_printerr("Error no se encontro el archivo :%s \n", error->message);
    g_clear_error(&error);
  }

  g_set_prgname("CAR");
  g_object_unref(G_OBJECT(constructor));
  gtk_main();
}

/**
 * @brief Build all GUI objects
 * @param GtkApplication pointer with
 */
ObjectsUI *buildObjects(GtkApplication *app) {
  GtkBuilder *constructor = gtk_builder_new();
  gtk_builder_add_from_file(constructor, "../gladeFiles/GUI_car.glade", NULL);
  ObjectsUI *obj = g_malloc(sizeof(ObjectsUI));

#ifdef DEBUG
  g_printerr("UI %p \n", obj);
#endif

  obj->Window = GTK_WIDGET(gtk_builder_get_object(constructor, "Window"));
  obj->GXLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis0"));
  obj->GXText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis0"));

  obj->GYLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis1"));
  obj->GYText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis1"));

  obj->GZLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis2"));
  obj->GZText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis2"));

  obj->AccelXLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis3"));
  obj->AccelXText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis3"));

  obj->AccelYLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis4"));
  obj->AccelYText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis4"));

  obj->AccelZLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis5"));
  obj->AccelZText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis5"));

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
void signalsConnection(ObjectsUI *obj, CARApp *app) { g_signal_connect_swapped(obj->Window, "destroy", G_CALLBACK(freeElements), app); }
