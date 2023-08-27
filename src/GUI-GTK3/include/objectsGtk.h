#ifndef _OBJECTSGTK_H_
#define _OBJECTSGTK_H_
#include "app.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

void start_gui(void);
ObjectsUI *buildObjects(GtkApplication *app);
void signalsConnection(ObjectsUI *obj, CARApp *app);
void freeElements(gpointer data);
gboolean move_image(gpointer data);
gboolean move_triangle(gpointer data);
gboolean on_draw_triangle(GtkWidget *widget,cairo_t *cr, gpointer data);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data); 
#endif