#ifndef __CAR_APP_H__
#define __CAR_APP_H__

#include "device.h"
#include <gtk/gtk.h>

#define CAT_TYPE_CAR_APP (car_app_get_type())
#define CAR_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), CAT_TYPE_CAR_APP, CARApp))

#define G_POINTER_TO_CAR_APP(p) ((CARApp *)(p))
typedef struct _ObjectsUI ObjectsUI;

struct _ObjectsUI {
  GtkWidget *Window;
  GtkWidget *ProgressBarAX;
  GtkWidget *ProgressBarAY;
  GtkWidget *ProgressBarAZ;
  GtkWidget *RangeCircle;
  GtkWidget *DrawingAreaCenterCircle;
  GdkPixbuf *imageCenterCircle;
  GtkWidget *DrawingAreaTriangle;
  GtkWidget *BarZaxis;
  GdkPixbuf *imageTriangle;
  GtkWidget *WindowTest;
  GtkWidget *RootDialog;
  GtkWidget *ExitDialogButton;
  GtkWidget *AcceptDialogButton;
  GtkWidget *DeviceStatusText;
  GtkWidget *DriverVersionText;
  GtkWidget *StartTestButton;
  GtkWidget *AnalysisProgressBar;
  GtkWidget *AutomaticTestCountdownText;
  GtkWidget *SaveAndStartAppButton;
  GtkWidget *LoadMpuConfigButton;
  GtkWidget *ExitTestButton;
  GtkWidget *DeviceStatusMainWindowText;
  GtkWidget *GUIVersionText;
  GtkWidget *TestDeviceButton;
  GtkWidget *OutputStatusTest;
  GtkTextBuffer *buffer;
  GtkTextIter iter;
  gdouble x_circle;
  gdouble y_circle;
  gdouble x_triangle;
  gdouble y_triangle;
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
