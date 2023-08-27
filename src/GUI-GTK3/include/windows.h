#ifndef _WINDOWS_H_
#define _WINDOWS_H_
#include <gtk/gtk.h>
#include "device.h"
#include "app.h"
void close_test_window(GtkWidget *,gpointer data);
void close_message(GtkWidget *,gpointer data);
void open_test_window(GtkWidget *,gpointer data);
void activate_elements(ObjectsUI *UI, Device *car);
void deactivate_elements(ObjectsUI *UI);
void start_timer(gpointer data);
gboolean UpdateLabelCountdown(gpointer data);
#endif