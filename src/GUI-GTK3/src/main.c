#include <gtk/gtk.h>

static void button_clicked(GtkWidget *widget, gpointer user_data) {
  g_print("Se hizo clic en el botón\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_fullscreen(GTK_WINDOW(window));
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

  GtkWidget *button = gtk_button_new_with_label("Haz clic");
  g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), NULL);
  gtk_container_add(GTK_CONTAINER(window), button);

  gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
