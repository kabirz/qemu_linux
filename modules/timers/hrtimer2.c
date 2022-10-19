#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timekeeping.h>

static struct hrtimer timer;
ktime_t kt;

u64 oldtc;

static enum hrtimer_restart hrtimer_hander(struct hrtimer *timer)
{
	u64 tc;

	printk("I am in hrtimer hander : %lu... \n", jiffies);
	tc = ktime_get_ns();

	printk("interval: %lld - %lld = %lld ns\n", tc, oldtc, tc - oldtc);
	oldtc = tc;
	hrtimer_forward(timer,timer->base->get_time(),kt);
	return HRTIMER_RESTART;
}

static int __init test_init(void)
{
	printk("---------test start-----------\n");

	oldtc = ktime_get_ns();
	kt = ktime_set(0,1000000);//1ms
	hrtimer_init(&timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
	hrtimer_start(&timer,kt,HRTIMER_MODE_REL);
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
