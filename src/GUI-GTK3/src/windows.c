#include "windows.h"
#include "objectsGtk.h"
#include <gtk/gtk.h>
#include <sys/ioctl.h>
gint8 SecondsToStartAutomaticTest = 10;

/**
 * Test Window
 */
void close_test_window(GtkWidget *, gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  gtk_widget_destroy(UI->WindowTest);
}

void open_test_window(GtkWidget *, gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;
  gtk_widget_show_all(UI->WindowTest);
  if (car->found == TRUE) {
    activate_elements(UI, car);
    start_timer(data);
  } else {
    deactivate_elements(UI);
  }
}

void activate_elements(ObjectsUI *UI, Device *car) { // TODO change name function to one more descriptive
  gtk_label_set_text(GTK_LABEL(UI->DeviceStatusText), "Connected");
  int8_t VersionNumber = 0;
  ioctl(car->fd, VERSION, &VersionNumber);
  char *version_str = g_strdup_printf("%d", VersionNumber);
  char *countdown_str = g_strdup_printf("%d", SecondsToStartAutomaticTest);
  gtk_label_set_text(GTK_LABEL(UI->DriverVersionText), version_str);
  gtk_label_set_text(GTK_LABEL(UI->AutomaticTestCountdownText), countdown_str);
  UI->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(UI->OutputStatusTest));
}

void deactivate_elements(ObjectsUI *UI) { gtk_label_set_text(GTK_LABEL(UI->DeviceStatusText), "Disconnected"); }

void start_timer(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  Device *car = CAR_APP(app)->priv->device;
  car->timer_countdown = g_timeout_add(1000, UpdateLabelCountdown, app);
}

gboolean UpdateLabelCountdown(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;
  SecondsToStartAutomaticTest--;
  char *number_str = g_strdup_printf("%d", SecondsToStartAutomaticTest);
  gtk_label_set_text(GTK_LABEL(UI->AutomaticTestCountdownText), number_str);
  if (SecondsToStartAutomaticTest == 0) {
    g_source_remove(car->timer_countdown);
    starting_test(data);
    SecondsToStartAutomaticTest = 5;
  }
  return TRUE;
}

void starting_test(gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  Device *car = CAR_APP(app)->priv->device;
  if (SecondsToStartAutomaticTest != 0) {
    g_source_remove(car->timer_countdown);
  }
  gtk_widget_set_sensitive(UI->StartTestButton, FALSE);
  gtk_label_set_text(GTK_LABEL(UI->AutomaticTestCountdownText), "0");
  print_status_text(data, "Starting test...\n");
}

void print_status_text(gpointer data, gchar *buffer) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  gtk_text_buffer_get_end_iter(UI->buffer, &UI->iter);
  gtk_text_buffer_insert(UI->buffer, &UI->iter, buffer, -1);
}

// *******************************

/**
 * Dialog Window (Popup)
 */
void close_message(GtkWidget *, gpointer data) {
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  ObjectsUI *UI = car_app_get_gui(app);
  gtk_widget_destroy(UI->RootDialog);
}
// *********************************
