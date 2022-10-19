#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>

static struct hrtimer timer;
ktime_t kt;

static enum hrtimer_restart hrtimer_hander(struct hrtimer *timer)
{
	static int count;

	printk("I am in hrtimer hander: %d\n", count++);
	hrtimer_forward(timer, timer->base->get_time(), kt);

	return HRTIMER_RESTART;
}

static int __init test_init(void)
{
	printk("---------%s-----------\n", __func__);

	kt = ktime_set(0, 1000000); // 0s  1000000ns  = 1ms
	hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_start(&timer, kt, HRTIMER_MODE_REL);
	timer.function = hrtimer_hander;
	return 0;
}

static void __exit test_exit(void)
{
	hrtimer_cancel(&timer);
	printk("------------test over---------------\n");
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
