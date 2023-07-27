
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
 *
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
 * @brief Draw steering wheel
 * @param GtkWidget widget to draw
 * @param cairo_t
 * @gpointer pointer with CARApp
 */
/*static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {

  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  cairo_save(cr);
  cairo_translate(cr, gtk_widget_get_allocated_width(widget) / 2.0, gtk_widget_get_allocated_height(widget) / 2.0);

  cairo_rotate(cr, UI->rotation);
  gdk_cairo_set_source_pixbuf(cr, UI->pixbuf, -gdk_pixbuf_get_width(UI->pixbuf) / 2.0, -gdk_pixbuf_get_height(UI->pixbuf) / 2.0);
  cairo_paint(cr);
  cairo_restore(cr);

  return TRUE;
}*/
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
  obj->RollLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis0"));
  obj->RollText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis0"));
  obj->PitchLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis1"));
  obj->PitchText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis1"));
  obj->YawLevelBar = GTK_WIDGET(gtk_builder_get_object(constructor, "LevelBarAxis2"));
  obj->YawText = GTK_WIDGET(gtk_builder_get_object(constructor, "Axis2"));
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
  Device *cae = CAR_APP(app)->priv->device;

#ifdef DEBUG
  g_printerr("%p UI\n", UI);
  g_printerr("%p device\n", cae);
#endif

  g_free(cae);
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
  g_signal_connect_swapped(obj->Window, "destroy", G_CALLBACK(freeElements), app);
  // g_signal_connect(G_OBJECT(obj->swa), "draw", G_CALLBACK(on_draw), app);
}
