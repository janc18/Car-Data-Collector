#ifndef _WINDOWS_H_
#define _WINDOWS_H_
#include "app.h"
#include "device.h"
#include <gtk/gtk.h>
void close_test_window(GtkWidget *, gpointer data);
void close_message(GtkWidget *, gpointer data);
void open_test_window(GtkWidget *, gpointer data);
void activate_elements(ObjectsUI *UI, Device *car);
void deactivate_elements(ObjectsUI *UI);
void start_timer(gpointer data);
gboolean UpdateLabelCountdown(gpointer data);
void starting_test(gpointer data);
void print_status_text(gpointer data, gchar *buffer);
#endif