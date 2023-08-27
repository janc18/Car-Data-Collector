#include <gtk/gtk.h>
#include "windows.h"
#include <sys/ioctl.h>
#include "objectsGtk.h"

/**
 * Test Window
 */
void close_test_window(GtkWidget *,gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
gtk_widget_destroy(UI->WindowTest);
}

/**
 * Dialog Window (Popup)
 */
void close_message(GtkWidget *,gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
gtk_widget_destroy(UI->RootDialog);
}


void open_test_window(GtkWidget *,gpointer data){
CARApp *app = G_POINTER_TO_CAR_APP(data);
ObjectsUI *UI = car_app_get_gui(app);
Device *car = CAR_APP(app)->priv->device;
gtk_widget_show_all(UI->WindowTest);
//If device found start countdown
if (car->found==TRUE){
  gtk_label_set_text(GTK_LABEL(UI->DeviceStatusText),"Connected");
  int8_t VersionNumber=10;
  ioctl(car->fd,VERSION,&VersionNumber);
  char *number_str = g_strdup_printf("%d", VersionNumber);
  gtk_label_set_text(GTK_LABEL(UI->DriverVersionText),number_str);
  }else{
  gtk_label_set_text(GTK_LABEL(UI->DeviceStatusText),"Disconnected");
}
}