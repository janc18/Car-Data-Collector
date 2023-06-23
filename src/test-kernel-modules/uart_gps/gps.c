#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/serdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jose Agustin");
MODULE_DESCRIPTION("A test driver for reading gps module");

static int serdev_gps_probe(struct serdev_device *serdev);
static void serdev_gps_remove(struct serdev_device *serdev);

static struct of_device_id serdev_gps_ids[] = {
    {
        .compatible = "brightlight,echodev",
    },
    {}};

MODULE_DEVICE_TABLE(of, serdev_gps_ids);

static struct serdev_device_driver serdev_gps_driver = {
    .probe = serdev_gps_probe,
    .remove = serdev_gps_remove,
    .driver =
        {
            .name = "serdev-gps",
            .of_match_table = serdev_gps_ids,
        },
};

static int serdev_gps_recv(struct serdev_device *serdev,
                           const unsigned char *buffer, size_t size) {
  printk("serdev_gps -received %u bytes with \"%s\"\n", size, buffer);
  return serdev_device_write_buf(serdev, buffer, size);
}

static const struct serdev_device_ops serdev_gps_ops = {
    .receive_buf = serdev_gps_recv,
};

static int serdev_gps_probe(struct serdev_device *serdev) {
  int status;
  printk("Probe funcion\n");

  serdev_device_set_client_ops(serdev, &serdev_gps_ops);
  status = serdev_device_open(serdev);

  if (status) {
    printk("serdev_gps - ERROR opening serial port\n");
    return -status;
  }

  serdev_device_set_baudrate(serdev, 9600);
  serdev_device_set_flow_control(serdev, false);
  serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);
  status = serdev_device_write_buf(serdev,
                                   "Escribe algo: ", sizeof("Escribe algo: "));
  printk("serdev_gps- Wrote %d bytes.\n", status);
  return 0;
}

static void serdev_gps_remove(struct serdev_device *serdev) {
  printk("Serdev_gps - Remove\n");
  serdev_device_close(serdev);
}

static int __init gps_init(void) {
  printk("serdev_gps - loading driver\n");
  if (serdev_device_driver_register(&serdev_gps_driver)) {
    printk("serdev_gps - ERROR, Could not load the driver \n");
    return -1;
  }
  return 0;
}

static void __exit gps_exit(void) {
  printk("serdev_gps - unload driver");
  serdev_device_driver_unregister(&serdev_gps_driver);
}

module_init(gps_init);
module_exit(gps_exit);
