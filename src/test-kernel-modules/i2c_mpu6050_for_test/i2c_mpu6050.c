#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/random.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
#define DRIVER_NAME "mpu6050"
#define DRIVER_CLASS "mpu6050Class"
static struct i2c_adapter *mpu6050_i2c_adapter = NULL;
static struct i2c_client *mpu6050_i2c_client = NULL;

MODULE_AUTHOR("Jose Agustin");
MODULE_DESCRIPTION("A test driver for reading out a MPU6050");

#define I2C_BUS_AVAILABLE 1
#define SLAVE_DEVICE_NAME "MPU6050"

// ioctl
#define GX _IOR('a', 'a', int16_t *)
#define GY _IOR('a', 'b', int16_t *)
#define GZ _IOR('a', 'c', int16_t *)
#define AX _IOR('a', 'd', int16_t *)
#define AY _IOR('a', 'e', int16_t *)
#define AZ _IOR('a', 'f', int16_t *)

// MPU6050 registers
const u8 MPU6050_SLAVE_ADDRESS = 0x68;
const u8 PWR_MGMT_1 = 0x6B;
const u8 GYRO_CONFIG = 0x1B;
const u8 ACCEL_CONFIG = 0x1C;
const u8 CONFIG = 0x1A;
//---------------

// Config registers
const u8 CLKSEL = 0x03;
const u8 FS_SEL = 0x03;
const u8 AFS_SEL = 0x03;
const u8 SLEEP = 0x00;
const u8 DLPF_CFG = 0x06;
//-------------

typedef struct RawMpuValues {
  u16 ACCEL_X;
  u16 ACCEL_Y;
  u16 ACCEL_Z;
  u16 GYRO_X;
  u16 GYRO_Y;
  u16 GYRO_Z;

} RawMpuValues;

// Range values to test the gui behavior
unsigned int max_value =15000; 
unsigned int min_value =32000; 

RawMpuValues mpu = {0, 0, 0, 0, 0, 0};
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
  pr_info("Read Function\n");
  return 0;
}

void get_raw_value(unsigned long arg, u16 *mpuValueRegister, u8 mpuRegister) {
    get_random_bytes(mpuValueRegister, sizeof(mpuValueRegister));
    *mpuValueRegister %= max_value;
    *mpuValueRegister += min_value;
  if (copy_to_user((u16 *)arg, mpuValueRegister, sizeof(u16))) {
    pr_err("Data Read : Err!\n");
  }
}

static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
  switch (cmd) {
  case GX:
    get_raw_value(arg, &mpu.GYRO_X, 67);
    break;
  case GY:
    get_raw_value(arg, &mpu.GYRO_Y, 69);
    break;
  case GZ:
    get_raw_value(arg, &mpu.GYRO_Z, 71);
    break;
  case AX:
    get_raw_value(arg, &mpu.ACCEL_X, 59);
    break;
  case AY:
    get_raw_value(arg, &mpu.ACCEL_Y, 61);
    break;
  case AZ:
    get_raw_value(arg, &mpu.ACCEL_Z, 63);
    break;

  default:
    pr_info("Default\n");
    break;
  }
  return 0;
}

static struct file_operations fops = {.owner = THIS_MODULE,
                                      .open = driver_open,
                                      .release = driver_close,
                                      .read = driver_read,
                                      .unlocked_ioctl = etx_ioctl};

static int __init ModuleInit(void) {

  int ret = -1;
  int id;

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
}
module_init(ModuleInit);
module_exit(ModuleExit);
