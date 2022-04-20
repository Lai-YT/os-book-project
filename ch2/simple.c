#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>


/**
 * @brief Module entry point: called when the module is loaded.
 * @return int: 0 representing success and any other value representing failure.
 */
int simple_init(void)
{
  /* priority flag */
  printk(KERN_INFO "Loading Module\n");

   return 0;
}

/**
 * @brief Module exit point: called when the module is removed.
 */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
