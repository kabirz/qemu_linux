#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/slab.h> // for kmalloc
#include <linux/fs.h> // for fops
#include <linux/uaccess.h> // for copy_to_user
struct cdev2_st {
	dev_t no;
	unsigned int user;
	struct cdev cdev;
	struct class *my_class;
	struct device *dev;
	char buf[1024];
};
static struct cdev2_st *cd;

static int cdev2_open(struct inode *inode, struct file *file)
{
	struct cdev2_st *st = container_of(inode->i_cdev, struct cdev2_st, cdev);

	file->private_data = (void *)st;
	st->user++;
	// dump_stack();

	return 0;
}

static int cdev2_release(struct inode *inode, struct file *file)
{
	struct cdev2_st *st = container_of(inode->i_cdev, struct cdev2_st, cdev);

	st->user--;
	// dump_stack();

	return 0;	
}

static ssize_t cdev2_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
	int ret = 0;
	struct cdev2_st *st = file->private_data;

	ret = copy_to_user(buf, st->buf, len);
	dev_dbg(st->dev, "Read: %s, len: %ld\n", st->buf, len);
	// dump_stack();

	return len - ret;
}

static ssize_t cdev2_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	int ret = 0;
	struct cdev2_st *st = file->private_data;

	ret = copy_from_user(st->buf, buf, len);
	dev_dbg(cd->dev, "Write: %s\n", st->buf);
	// dump_stack();

	return len - ret;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = cdev2_open,
	.release = cdev2_release,
	.write = cdev2_write,
	.read = cdev2_read,
};

static __init int cdev2_init(void)
{
	int ret;
	cd = kmalloc(sizeof(struct cdev2_st), GFP_KERNEL);
	if (cd == NULL) {
		pr_err("malloc error!\n");
		return -ENOMEM;
	}
	memset(cd, 0, sizeof(*cd));
	cd->no = MKDEV(122, 1);
	register_chrdev(122, "tc1", &fops);

	cdev_init(&cd->cdev, &fops);
	ret = cdev_add(&cd->cdev, cd->no, 1);
	if (ret) {
		pr_err("cdev add error!\n");
		return ret;
	}
	cd->my_class = class_create(THIS_MODULE, "my");
	cd->dev = device_create(cd->my_class, NULL, cd->no, NULL, "tc1");
	printk("Hello, load linux driver!: %p\n", cd->dev);
	return 0;
}

static __exit void cdev2_deinit(void)
{
	device_destroy(cd->my_class, cd->no);
	class_destroy(cd->my_class);
	kfree(cd);
	printk("Goodbye, exit linux driver!\n");
}

module_init(cdev2_init);
module_exit(cdev2_deinit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhong Huiping");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("cdev2 driver");
