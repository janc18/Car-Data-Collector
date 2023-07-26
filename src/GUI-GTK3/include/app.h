#ifndef __CAR_APP_H__
#define __CAR_APP_H__

#include "device.h"
#include <gtk/gtk.h>

#define CAT_TYPE_CAR_APP (car_app_get_type())
#define CAR_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), CAT_TYPE_CAR_APP, CARApp))

#define G_POINTER_TO_CAR_APP(p) ((CARApp *)(p))
typedef struct _ObjectsUI ObjectsUI;

struct _ObjectsUI {
  GtkWidget *ventana;
  GtkWidget *visual_status;
  GtkWidget *text_status;
  GtkWidget *boton;
  GtkWidget *etiqueta;
  GtkWidget *barra_acelerador;
  GtkWidget *barra_freno;
  GtkWidget *barra_clutch;
  GtkWidget *output;
  GtkWidget *swa; // sterring wheel animation
  gdouble rotation;
  GtkWidget *reconectar;
  GtkWidget *search_updates; // button
  GtkWidget *tv_updatelog;
  GdkPixbuf *pixbuf;
};

typedef struct _CARAppPrivate CARAppPrivate;
typedef struct _CARApp CARApp;
typedef struct _CARAppClass CARAppClass;

struct _CARAppPrivate {
  ObjectsUI *UI;
  Device *device;
  guint most_recent_id;
};

struct _CARApp {
  GtkApplication parent;
  CARAppPrivate *priv;
};

struct _CARAppClass {
  GtkApplicationClass parent_class;
};

ObjectsUI *car_app_get_gui(CARApp *app);
GtkApplication *car_app_new(void);

GType car_app_get_type(void);
#endif
