#include <linux/init.h>
#include <linux/module.h>

#ifdef MODULE
__init int init_module(void)
#else
 static __init int cdev1_init(void)
#endif
{
	printk("Hello, load linux driver!\n");
	return 0;
}

#ifdef MODULE
__exit void cleanup_module(void)
#else
static __exit void cdev1_deinit(void)
#endif
{
	printk("goodbye, exit linux driver!\n");
}

#ifndef MODULE
module_init(cdev1_init);
module_exit(cdev1_deinit);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhong Huiping");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("test driver");
