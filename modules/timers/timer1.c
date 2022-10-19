#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>

struct timer_st {
	struct timer_list list;
} st;


static void timer1_cb(struct timer_list *t)
{
	static int count = 0;
	pr_info("%s: %d, %d\n", __func__, __LINE__, count++);
	t->expires = jiffies + msecs_to_jiffies(2000);
	add_timer(t);
}

 static __init int timer1_init(void)
{
	printk("Hello, load linux driver!\n");
	timer_setup(&st.list, timer1_cb, 0);
	st.list.expires = jiffies + msecs_to_jiffies(1000);
	add_timer(&st.list);
	return 0;
}

static __exit void timer1_deinit(void)
{
	del_timer(&st.list);
	printk("goodbye, exit linux driver!\n");
}

module_init(timer1_init);
module_exit(timer1_deinit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhong Huiping");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("timer test driver");
