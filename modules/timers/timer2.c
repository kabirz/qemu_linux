#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/uaccess.h>

#define TIMER_COUNT 1
#define TIMER_NAME "timer"

#define CLOSE_CMD _IO(0XEF, 1)
#define OPEN_CMD _IO(0XEF, 2)
#define SETPERIOD_CMD _IOW(0XEF, 3, int)

struct timer_dev {
	dev_t devid;
	int major;
	int minor;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	struct device_node *node;
	int led_gpio;
	int timerperiod;
	struct timer_list timer;
	spinlock_t lock;
};

struct timer_dev timer;

static int timer_open(struct inode *inode, struct file *filp)
{
	filp->private_data = &timer;

	return 0;
}

static long timer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int value = 0;
	struct timer_dev *dev = (struct timer_dev *)filp->private_data;

	switch (cmd) {
		case CLOSE_CMD:
			del_timer_sync(&dev->timer);
			break;
		case OPEN_CMD:
			mod_timer(&dev->timer, jiffies + msecs_to_jiffies(timer.timerperiod));
			break;
		case SETPERIOD_CMD:
			ret = copy_from_user(&value, (int *)arg, sizeof(int));
			if (ret < 0) {
				return -EFAULT;
			}
			dev->timerperiod = value;
			mod_timer(&dev->timer, jiffies + msecs_to_jiffies(timer.timerperiod));
			break;
	}

	return ret;
}

static int timer_release(struct inode *inode, struct file *filp) { return 0; }

static const struct file_operations timer_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = timer_ioctl,
	.open = timer_open,
	.release = timer_release,

};

static void timer_func(struct timer_list *list)
{
	struct timer_dev *dev = container_of(list, struct timer_dev, timer);
	static int sta = 1;
	unsigned long flags;

	sta = !sta;
	spin_lock_irqsave(&dev->lock, flags);
	gpio_set_value(dev->led_gpio, sta);
	spin_unlock_irqrestore(&dev->lock, flags);
	mod_timer(&dev->timer, jiffies + msecs_to_jiffies(timer.timerperiod));
}

int led_init(struct timer_dev *dev)
{
	int ret = 0;

	dev->node = of_find_node_by_path("/gpioled");
	if (dev->node == NULL) {
		pr_err("not found led gpio\n");
		ret = -EINVAL;
		goto failed_findnode;
	}

	dev->led_gpio = of_get_named_gpio(dev->node, "led-gpios", 0);
	if (dev->led_gpio < 0) {
		pr_info("can't find led gpio \r\n");
		ret = -EINVAL;
		goto failed_findnode;
	}

	pr_info("led gpio num = %d \r\n", dev->led_gpio);

	ret = gpio_request(dev->led_gpio, "led-gpios");
	if (ret) {
		pr_info("Failed to request gpio \r\n");
		ret = -EINVAL;
		goto failed_findnode;
	}

	ret = gpio_direction_output(dev->led_gpio, 1);
	if (ret < 0) {
		goto failed_setoutput;
	}

	return 0;

failed_setoutput:
	gpio_free(dev->led_gpio);
failed_findnode:
	device_destroy(dev->class, dev->devid);
	return ret;
}

static int __init timer_init(void)
{
	int ret = 0;

	spin_lock_init(&timer.lock);

	timer.major = 0;

	if (timer.major) {
		timer.devid = MKDEV(timer.major, 0);
		ret = register_chrdev_region(timer.devid, TIMER_COUNT, TIMER_NAME);
	} else {
		ret = alloc_chrdev_region(&timer.devid, 0, TIMER_COUNT, TIMER_NAME);
		timer.major = MAJOR(timer.devid);
		timer.minor = MINOR(timer.devid);
	}

	if (ret < 0) {
		goto failed_devid;
	}
	pr_info("timerdev major = %d minor = %d \r\n", timer.major, timer.minor);

	timer.cdev.owner = THIS_MODULE;
	cdev_init(&timer.cdev, &timer_fops);
	ret = cdev_add(&timer.cdev, timer.devid, TIMER_COUNT);
	if (ret < 0) {
		goto failed_cdev;
	}

	timer.class = class_create(THIS_MODULE, TIMER_NAME);
	if (IS_ERR(timer.class)) {
		ret = PTR_ERR(timer.class);
		goto failed_class;
	}

	timer.device = device_create(timer.class, NULL, timer.devid, NULL, TIMER_NAME);
	if (IS_ERR(timer.device)) {
		ret = PTR_ERR(timer.device);
		pr_err("device create failed\n");
		goto failed_device;
	}

	ret = led_init(&timer);
	if (ret < 0) {
		pr_err("led_init failed\n");
		goto failed_ledinit;
	}

	timer_setup(&timer.timer, timer_func, 0);
	timer.timer.expires = jiffies + msecs_to_jiffies(timer.timerperiod);
	add_timer(&timer.timer);

	return 0;

failed_ledinit:
failed_device:
	class_destroy(timer.class);
failed_class:
	cdev_del(&timer.cdev);
failed_cdev:
	unregister_chrdev_region(timer.devid, TIMER_COUNT);
failed_devid:
	return ret;
}

static void __exit timer_exit(void) {

	del_timer(&timer.timer);

	gpio_free(timer.led_gpio);
	cdev_del(&timer.cdev);
	unregister_chrdev_region(timer.devid, TIMER_COUNT);

	device_destroy(timer.class, timer.devid);
	class_destroy(timer.class);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZYC");
