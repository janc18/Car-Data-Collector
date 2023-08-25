#ifndef __DRAWING_H__
#define __DRAWING_H__
#include <gtk/gtk.h>
gboolean move_image(gpointer data);
gboolean move_triangle(gpointer data);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean on_draw_triangle(GtkWidget *widget,cairo_t *cr, gpointer data);
#endif