#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
MODULE_LICENSE("GPL");
#define DRIVER_NAME "mpu6050"
#define DRIVER_CLASS "mpu6050Class"

static struct i2c_adapter *mpu6050_i2c_adapter = NULL;
static struct i2c_client *mpu6050_i2c_client = NULL;

MODULE_AUTHOR("Jose Agustin");
MODULE_DESCRIPTION("A test driver for reading out a MPU6050");
// MODULE_SUPPORTED_DEVICE("NONE");

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_NAME "MPU6050"
#define MPU6050_SLAVE_ADDRESS 0x68

static const struct i2c_device_id mpu6050_id[] = {{SLAVE_DEVICE_NAME, 0}, {}};
static struct i2c_driver mpu6050_driver = { // it have const before
    .driver = {.name = SLAVE_DEVICE_NAME, .owner = THIS_MODULE}};

static struct i2c_board_info mpu6050_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, MPU6050_SLAVE_ADDRESS)};

static dev_t myDeviceNr;
static struct class *myClass;
static struct cdev myDevice;

static int driver_open(struct inode *deviceFile, struct file *instance) {
  printk("My Device driver - open was called\n");
  return 0;
}

static int driver_close(struct inode *deviceFile, struct file *instance) {
  printk("My Device driver - close was called\n");
  return 0;
}

u8 get_whoam(void) {
  u8 register_whoam;
  register_whoam = i2c_smbus_read_byte_data(mpu6050_i2c_client, 0x75);
  return register_whoam;
}


static ssize_t driver_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char out_string[10];
    u8 register_value;

    if (*offs >= sizeof(out_string))
        return 0;

    register_value = get_whoam();
    snprintf(out_string, sizeof(out_string), "%d\n", register_value);

    to_copy = min_t(int, strlen(out_string) - *offs, count);
    if (to_copy <= 0)
        return 0;

    not_copied = copy_to_user(user_buffer, out_string + *offs, to_copy);
    delta = to_copy - not_copied;

    *offs += delta;

    return delta;
 }

static struct file_operations fops = {.owner = THIS_MODULE,
                                      .open = driver_open,
                                      .release = driver_close,
                                      .read = driver_read};
static int __init ModuleInit(void) {
  int ret = -1;
  int id;
  printk("My device Driver - hello kernel\n");

  if (alloc_chrdev_region(&myDeviceNr, 0, 1, DRIVER_NAME) < 0) {
    printk("Device NR,could not be allocater\n");
  }
  printk("My device driver -device NR %d was register\n", myDeviceNr);
  if ((myClass = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
    printk("Device class cannot be created\n");
    goto ClassError;
  }

  if (device_create(myClass, NULL, myDeviceNr, NULL, DRIVER_NAME) == NULL) {
    printk("cannot create device file\n");
    goto FileError;
  }
  cdev_init(&myDevice, &fops);

  if (cdev_add(&myDevice, myDeviceNr, 1) == -1) {
    printk("Register of device to kernel failed\n");
    goto KernelError;
  }
  mpu6050_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
  if (mpu6050_i2c_adapter != NULL) {
    mpu6050_i2c_client =
        i2c_new_client_device(mpu6050_i2c_adapter, &mpu6050_i2c_board_info);
    if (mpu6050_i2c_client != NULL) {
      if (i2c_add_driver(&mpu6050_driver) != -1) {
        ret = 0;
      } else
        printk("cannot add driver\n");
    }
    i2c_put_adapter(mpu6050_i2c_adapter);
  }
  printk("MPU Driver added!!!\n");
  id = i2c_smbus_read_byte_data(mpu6050_i2c_client, 0x75);
  return ret;
KernelError:
  device_destroy(myClass, myDeviceNr);
FileError:
  class_destroy(myClass);
ClassError:
  unregister_chrdev(myDeviceNr, DRIVER_NAME);
  return -1;
}

static void __exit ModuleExit(void) {
  printk("mpu6050 dismonted\n");
  i2c_unregister_device(mpu6050_i2c_client);
  i2c_del_driver(&mpu6050_driver);
  cdev_del(&myDevice);
  device_destroy(myClass, myDeviceNr);
  class_destroy(myClass);
  unregister_chrdev_region(myDeviceNr, 1);
}
module_init(ModuleInit);
module_exit(ModuleExit);
