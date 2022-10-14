#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static __init int hello_init(void)
{
	//unsigned int cpu = get_cpu();
	struct module *mod;
		
	pr_info("this module: %p==%p\n", &__this_module, THIS_MODULE);
	pr_info("module state: %d\n", THIS_MODULE->state);
	pr_info("module name: %s\n", THIS_MODULE->name);

	list_for_each_entry(mod, *(&THIS_MODULE->list.prev), list)
		pr_info("module name: %s\n", mod->name);
	return 0;
}

static __exit void hello_exit(void)
{
	pr_info("module state: %d\n", THIS_MODULE->state);
}

module_init(hello_init);
module_exit(hello_exit);
