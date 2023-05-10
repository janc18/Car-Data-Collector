#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
MODULE_AUTHOR("Jose Agustin");
MODULE_DESCRIPTION("Car Data collector");
MODULE_LICENSE("MIT");
static int __init custom_init(void) { return 0; }
static void __exit custom_exit(void) {}
module_init(custom_init);
module_exit(custom_exit);
