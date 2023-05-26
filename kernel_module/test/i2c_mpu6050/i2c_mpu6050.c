#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
#define DRIVER_NAME "mpu6050"
#define DRIVER_CLASS "mpu6050Class"
#define TIMEOUT 5000 // milliseconds
static struct i2c_adapter *mpu6050_i2c_adapter = NULL;
static struct i2c_client *mpu6050_i2c_client = NULL;

MODULE_AUTHOR("Jose Agustin");
MODULE_DESCRIPTION("A test driver for reading out a MPU6050");
// MODULE_SUPPORTED_DEVICE("NONE");

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_NAME "MPU6050"
#define MPU6050_SLAVE_ADDRESS 0x68

// MPU6050 registers

const u8 PWR_MGMT_1 = 0x6B;
const u8 GYRO_CONFIG = 0x1B;
const u8 ACCEL_CONFIG = 0x1C;
const u8 CONFIG = 0x1A;
//---------------

// Config registers
const u8 CLKSEL = 0x01;
const u8 FS_SEL = 0x00;
const u8 AFS_SEL = 0x00;
const u8 SLEEP = 0x00;
const u8 DLPF_CFG = 0x06;
//-------------

static char *values_format = NULL;

// kthread
static struct task_struct *read_thread;

//--

// MPU6050 gyroscope and acelerometer registers

// Fuction Prototypes

int read_accel_raw_values(void *pv) {
  while (!kthread_should_stop()) {
    u16 ACCEL_X = 0;
    u16 ACCEL_Y = 0;
    u16 ACCEL_Z = 0;
    u16 GYRO_X = 0;
    u16 GYRO_Y = 0;
    u16 GYRO_Z = 0;
    ACCEL_X = i2c_smbus_read_word_data(mpu6050_i2c_client, 59);
    ACCEL_Y = i2c_smbus_read_word_data(mpu6050_i2c_client, 61);
    ACCEL_Z = i2c_smbus_read_word_data(mpu6050_i2c_client, 63);
    GYRO_X = i2c_smbus_read_word_data(mpu6050_i2c_client, 67);
    GYRO_Y = i2c_smbus_read_word_data(mpu6050_i2c_client, 69);
    GYRO_Z = i2c_smbus_read_word_data(mpu6050_i2c_client, 71);

    snprintf(values_format, 80, "%d|%d|%d|%d|%d|%d", ACCEL_X, ACCEL_Y, ACCEL_Z,
             GYRO_X, GYRO_Y, GYRO_Z);
    msleep(1000);
  }
  return 0;
}
//-------

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

void config_mpu6050(void) {
  i2c_smbus_write_byte_data(mpu6050_i2c_client, PWR_MGMT_1,
                            CLKSEL << 2 | SLEEP << 6);
  i2c_smbus_write_byte_data(mpu6050_i2c_client, GYRO_CONFIG, FS_SEL << 3);
  i2c_smbus_write_byte_data(mpu6050_i2c_client, ACCEL_CONFIG, AFS_SEL << 2);
  i2c_smbus_write_byte_data(mpu6050_i2c_client, CONFIG, DLPF_CFG);
}

static ssize_t driver_read(struct file *file, char __user *user_buffer,
                           size_t count, loff_t *offs) {
  int to_copy, not_copied, delta;
  char out_string[60];

  if (*offs >= sizeof(out_string))
    return 0;

  snprintf(out_string, sizeof(out_string), "%s\n", values_format);
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
  // allocated memory for output format

  int ret = -1;
  int id;
  values_format = vmalloc(100);

  if (alloc_chrdev_region(&myDeviceNr, 0, 1, DRIVER_NAME) < 0) {
    printk("MPU6050: Device NR,could not be allocater\n");
  }
  printk("MPU6050: My device driver -device NR %d was register\n", myDeviceNr);
  if ((myClass = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
    printk("MPU6050: Device class cannot be created\n");
    goto ClassError;
  }

  if (device_create(myClass, NULL, myDeviceNr, NULL, DRIVER_NAME) == NULL) {
    printk("MPU6050: canot create device file\n");
    goto FileError;
  }
  cdev_init(&myDevice, &fops);

  if (cdev_add(&myDevice, myDeviceNr, 1) == -1) {
    printk("MPU6050: Register of device to kernel failed\n");
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
        printk("MPU6050: cannot add driver\n");
    }
    i2c_put_adapter(mpu6050_i2c_adapter);
  }
  printk("MPU6050: MPU Driver added!!!\n");
  id = i2c_smbus_read_byte_data(mpu6050_i2c_client, 0x75);
  if (id == 0x68) {
    printk("MPU6050: Mpu6050 found -- Initializing configuration \n");
    config_mpu6050();
  } else {
    printk("MPU6050: Mpu6050 not found");
  }
  read_thread =
      kthread_create(read_accel_raw_values, NULL, "raw values mpu read");
  if (read_thread) {
    wake_up_process(read_thread);
  } else {
    pr_err("MPU6050: cannot create driver");
    goto KernelError;
  }
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
  printk("MPU6050: Mpu6050 dismonted\n");
  i2c_unregister_device(mpu6050_i2c_client);
  i2c_del_driver(&mpu6050_driver);
  cdev_del(&myDevice);
  device_destroy(myClass, myDeviceNr);
  class_destroy(myClass);
  unregister_chrdev_region(myDeviceNr, 1);
  vfree(values_format);
  kthread_stop(read_thread);
}
module_init(ModuleInit);
module_exit(ModuleExit);
