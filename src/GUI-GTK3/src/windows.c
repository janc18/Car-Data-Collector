#include <gtk/gtk.h>
#include "windows.h"
#include <sys/ioctl.h>
#include "objectsGtk.h"
gint8 SecondsToStartAutomaticTest =10;

/**
 * Test Window
 */
void close_test_window(GtkWidget *,gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
gtk_widget_destroy(UI->WindowTest);
}

void open_test_window(GtkWidget *,gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
Device *car = CAR_APP(app)->priv->device;
gtk_widget_show_all(UI->WindowTest);
if (car->found==TRUE){
  activate_elements(UI,car);
  start_timer(data);
  }else{
  deactivate_elements(UI);
}
}

void activate_elements(ObjectsUI *UI, Device *car){//TODO change name function to one more descriptive
  gtk_label_set_text(GTK_LABEL(UI->DeviceStatusText),"Connected");
  int8_t VersionNumber=0;
  ioctl(car->fd,VERSION,&VersionNumber);
  char *version_str = g_strdup_printf("%d", VersionNumber);
  char *countdown_str = g_strdup_printf("%d", SecondsToStartAutomaticTest);
  gtk_label_set_text(GTK_LABEL(UI->DriverVersionText),version_str);
  gtk_label_set_text(GTK_LABEL(UI->AutomaticTestCountdownText),countdown_str);
}

void deactivate_elements(ObjectsUI *UI){
  gtk_label_set_text(GTK_LABEL(UI->DeviceStatusText),"Disconnected");
}

void start_timer(gpointer data){
  CARApp *app = G_POINTER_TO_CAR_APP(data);
  Device *car = CAR_APP(app)->priv->device;
  car->timer_countdown= g_timeout_add(1000, UpdateLabelCountdown, app);
}

gboolean UpdateLabelCountdown(gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
Device *car = CAR_APP(app)->priv->device;
SecondsToStartAutomaticTest--;
char *number_str = g_strdup_printf("%d", SecondsToStartAutomaticTest);
gtk_label_set_text(GTK_LABEL(UI->AutomaticTestCountdownText),number_str);
if(SecondsToStartAutomaticTest==0){
g_source_remove(car->timer_countdown);
SecondsToStartAutomaticTest=5;
}
return TRUE;
}
// *******************************

/**
 * Dialog Window (Popup)
 */
void close_message(GtkWidget *,gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
gtk_widget_destroy(UI->RootDialog);
}
// *********************************

