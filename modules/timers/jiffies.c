#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>

struct timer_st {
	struct timer_list list;
} st;


 static __init int jfs_init(void)
{
	printk("Hello, load linux driver!\n");
	printk("ms: %d\n ", jiffies_to_msecs(jiffies));
	return 0;
}

static __exit void jfs_deinit(void)
{
	printk("goodbye, exit linux driver!\n");
}

module_init(jfs_init);
module_exit(jfs_deinit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhong Huiping");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("jiffies test driver");
