#include <gtk/gtk.h>
#include "objectsGtk.h"
#include "app.h"
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

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);

  if (UI->imageCenterCircle != NULL) {
    #ifdef DEBUG
    g_printerr("x=%f y=%f\n", UI->x_circle, UI->y_circle);
    #endif
    gdk_cairo_set_source_pixbuf(cr, UI->imageCenterCircle, UI->x_circle, UI->y_circle);
    cairo_paint(cr);
  }
  return TRUE;
}

gboolean on_draw_triangle(GtkWidget *widget,cairo_t *cr, gpointer data){
  CARApp *app =G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI =car_app_get_gui(app);
  if (UI->imageTriangle != NULL){
    #ifdef DEBUG
    g_printerr("Triangle x=%f Triangle y=%f\n",UI->x_triangle,UI->y_triangle);
    #endif
    gdk_cairo_set_source_pixbuf(cr,UI->imageTriangle,UI->x_triangle,UI->y_triangle);
    cairo_paint(cr);
  }else{
    g_printerr("error in triangle\n");
  }
  return TRUE;
}