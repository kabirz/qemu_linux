## arm64 设备树分析

从Linux启动流程上分析：start_kernel -> setup_arch -> setup_machine_fdt(__fdt_pointer) unflatten_device_tree  
__fdt_pointer为bootloader传上来的设备树存放的物理地址。
```c
// init/main.c
asmlinkage __visible void __init __no_sanitize_address start_kernel(void)
{
	char *command_line;
	char *after_dashes;

	set_task_stack_end_magic(&init_task);
	smp_setup_processor_id();
	debug_objects_early_init();
	init_vmlinux_build_id();
	...
	pr_notice("%s", linux_banner);
	early_security_init();
	setup_arch(&command_line); // <<<<<<<<<<<<<<<<<<<<<<<
	setup_boot_config();
	setup_command_line(command_line);
	setup_nr_cpu_ids();
	setup_per_cpu_areas();
	...
}

```
linux启动后start_kernel会对平台初始化setup_arch
```c
// arch/arm64/kernel/setup.c
void __init __no_sanitize_address setup_arch(char **cmdline_p)
{
	setup_initial_init_mm(_stext, _etext, _edata, _end);

	*cmdline_p = boot_command_line;

	/*
	 * If know now we are going to need KPTI then use non-global
	 * mappings from the start, avoiding the cost of rewriting
	 * everything later.
	 */
	arm64_use_ng_mappings = kaslr_requires_kpti();

	early_fixmap_init();
	early_ioremap_init();

	setup_machine_fdt(__fdt_pointer); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	jump_label_init();
	parse_early_param();


	if (acpi_disabled)
		unflatten_device_tree(); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	.....
}
```

unflatten_device_tree函数对设备树进行解析, 根据设备树node创建所有的device_node

```c
// drivers/of/fdt.c 
void __init unflatten_device_tree(void)
{
	__unflatten_device_tree(initial_boot_params, NULL, &of_root,
				early_init_dt_alloc_memory_arch, false);

	/* Get pointer to "/chosen" and "/aliases" nodes for use everywhere */
	of_alias_scan(early_init_dt_alloc_memory_arch);

	unittest_unflatten_overlay_base();
}
```

我们最后的目标是希望在后续获取到设备树上的信息，因此linux 会为device_node创建 platform_device

```c
// drivers/of/platform.c
static int __init of_platform_default_populate_init(void)
{
	struct device_node *node;

	device_links_supplier_sync_state_pause();

	if (!of_have_populated_dt())
		return -ENODEV;

	if (IS_ENABLED(CONFIG_PPC)) {
		...
	} else {
		...
		node = of_get_compatible_child(of_chosen, "simple-framebuffer");
		of_platform_device_create(node, NULL, NULL);
		of_node_put(node);

		/* Populate everything else. */
		of_platform_default_populate(NULL, NULL, NULL); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	}

	return 0;
}
arch_initcall_sync(of_platform_default_populate_init);
```

of_platform_default_populate会为所有的device_node 创建 platform_device

```c
// drivers/of/platform.c
int of_platform_default_populate(struct device_node *root,
				 const struct of_dev_auxdata *lookup,
				 struct device *parent)
{
	return of_platform_populate(root, of_default_bus_match_table, lookup,
				    parent);
}

int of_platform_populate(struct device_node *root,
			const struct of_device_id *matches,
			const struct of_dev_auxdata *lookup,
			struct device *parent)
{
	struct device_node *child;
	int rc = 0;

	root = root ? of_node_get(root) : of_find_node_by_path("/");
	if (!root)
		return -EINVAL;

	pr_debug("%s()\n", __func__);
	pr_debug(" starting at: %pOF\n", root);

	device_links_supplier_sync_state_pause();
	for_each_child_of_node(root, child) {
		rc = of_platform_bus_create(child, matches, lookup, parent, true); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		if (rc) {
			of_node_put(child);
			break;
		}
	}
	device_links_supplier_sync_state_resume();

	of_node_set_flag(root, OF_POPULATED_BUS);

	of_node_put(root);
	return rc;
}
```

所有的设备树的节点都创建了对应的platform_device, 后续platform driver会对去匹配 plaform_device  
以drivers/i2c/busses/i2c-versatile.c 为例

```c
// drivers/i2c/busses/i2c-versatile.c
static const struct of_device_id i2c_versatile_match[] = {
	{ .compatible = "arm,versatile-i2c", },
	{},
};
MODULE_DEVICE_TABLE(of, i2c_versatile_match);

static struct platform_driver i2c_versatile_driver = {
	.probe		= i2c_versatile_probe,
	.remove		= i2c_versatile_remove,
	.driver		= {
		.name	= "versatile-i2c",
		.of_match_table = i2c_versatile_match,
	},
};

static int __init i2c_versatile_init(void)
{
	return platform_driver_register(&i2c_versatile_driver);
}

static void __exit i2c_versatile_exit(void)
{
	platform_driver_unregister(&i2c_versatile_driver);
}

subsys_initcall(i2c_versatile_init);
module_exit(i2c_versatile_exit);
```

i2c_versatile_init -> platform_driver_register -> driver_register -> bus_add_driver -> driver_attach -> match -> platform_match

```c
// drivers/base/platform.c
static int platform_match(struct device *dev, struct device_driver *drv)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct platform_driver *pdrv = to_platform_driver(drv);

	/* When driver_override is set, only bind to the matching driver */
	if (pdev->driver_override)
		return !strcmp(pdev->driver_override, drv->name);

	/* Attempt an OF style match first */
	if (of_driver_match_device(dev, drv)) // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		return 1;

	/* Then try ACPI style match */
	if (acpi_driver_match_device(dev, drv))
		return 1;

	/* Then try to match against the id table */
	if (pdrv->id_table)
		return platform_match_id(pdrv->id_table, pdev) != NULL;

	/* fall-back to driver name match */
	return (strcmp(pdev->name, drv->name) == 0);
}
```

在 driver_attach函数中会遍历整个设备树生成的platform_device去匹配  
如果我们的设备树中存在和源码中
```c
static const struct of_device_id i2c_versatile_match[] = {
	{ .compatible = "arm,versatile-i2c", },
	{},
};
```
相同的compatible，则i2c-versatile的驱动和设备就匹配上了，将会执行probe函数, 查看设备树发现存在
```dts
i2c@2000 {
	compatible = "arm,versatile-i2c";
	reg = <0x2000 0x1000>;
	#address-cells = <0x01>;
	#size-cells = <0x00>;

	pcie-switch@60 {
		compatible = "idt,89hpes32h8";
		reg = <0x60>;
	};
};
```

同时在i2c的节点下挂有一个设备地址为0x60的设备，之后i2c bus driver 会去把 i2c 设备的驱动也匹配  
流程： i2c_versatile_probe -> i2c_bit_add_numbered_bus -> __i2c_bit_add_bus -> add_adapter ->
i2c_add_adapter -> i2c_register_adapter -> of_i2c_register_devices -> of_i2c_get_board_info

```c
// drivers/i2c/busses/i2c-versatile.c
static int i2c_versatile_probe(struct platform_device *dev)
{
	struct i2c_versatile *i2c;
	struct resource *r;
	int ret;

	i2c = devm_kzalloc(&dev->dev, sizeof(struct i2c_versatile), GFP_KERNEL);
	if (!i2c)
		return -ENOMEM;
	...
	i2c->adap.owner = THIS_MODULE;
	strscpy(i2c->adap.name, "Versatile I2C adapter", sizeof(i2c->adap.name));
	i2c->adap.algo_data = &i2c->algo;
	i2c->adap.dev.parent = &dev->dev;
	i2c->adap.dev.of_node = dev->dev.of_node; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 把设个device_node 放到 adpater中以便i2c match的时候使用
	i2c->algo = i2c_versatile_algo;
	i2c->algo.data = i2c;

	i2c->adap.nr = dev->id;
	ret = i2c_bit_add_numbered_bus(&i2c->adap); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (ret < 0)
		return ret;

	platform_set_drvdata(dev, i2c);

	return 0;
}
```

```c
// drivers/i2c/i2c-core-base.c
int i2c_add_numbered_adapter(struct i2c_adapter *adap)
{
	if (adap->nr == -1) /* -1 means dynamically assign bus id */
		return i2c_add_adapter(adap); // <<<<<<<<<<<<<<<<<<<<<<

	return __i2c_add_numbered_adapter(adap); // <<<<<<<<<<<<<<<<<<<
}
// drivers/i2c/algos/i2c-algo-bit.c
int i2c_bit_add_numbered_bus(struct i2c_adapter *adap)
{
	return __i2c_bit_add_bus(adap, i2c_add_numbered_adapter); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}
// drivers/i2c/i2c-core-base.c
int i2c_add_adapter(struct i2c_adapter *adapter)
{
	struct device *dev = &adapter->dev;
	int id;

	if (dev->of_node) {
		id = of_alias_get_id(dev->of_node, "i2c");
		if (id >= 0) {
			adapter->nr = id;
			return __i2c_add_numbered_adapter(adapter); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		}
	}

	mutex_lock(&core_lock);
	id = idr_alloc(&i2c_adapter_idr, adapter,
		       __i2c_first_dynamic_bus_num, 0, GFP_KERNEL);
	mutex_unlock(&core_lock);
	if (WARN(id < 0, "couldn't get idr"))
		return id;

	adapter->nr = id;

	return i2c_register_adapter(adapter); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
}

// drivers/i2c/i2c-core-base.c
static int i2c_register_adapter(struct i2c_adapter *adap)
{
	...
	/* create pre-declared device nodes */
	of_i2c_register_devices(adap); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	i2c_acpi_install_space_handler(adap);
	i2c_acpi_register_devices(adap);

	if (adap->nr < __i2c_first_dynamic_bus_num)
		i2c_scan_static_board_info(adap);

	/* Notify drivers */
	mutex_lock(&core_lock);
	bus_for_each_drv(&i2c_bus_type, NULL, adap, __process_new_adapter);
	mutex_unlock(&core_lock);

	return 0;
	...

}

// drivers/i2c/i2c-core-of.c
void of_i2c_register_devices(struct i2c_adapter *adap)
{
	struct device_node *bus, *node;
	struct i2c_client *client;

	/* Only register child devices if the adapter has a node pointer set */
	if (!adap->dev.of_node)
		return;

	dev_dbg(&adap->dev, "of_i2c: walking child nodes\n");

	bus = of_get_child_by_name(adap->dev.of_node, "i2c-bus");
	if (!bus)
		bus = of_node_get(adap->dev.of_node);

	for_each_available_child_of_node(bus, node) {
		if (of_node_test_and_set_flag(node, OF_POPULATED))
			continue;

		client = of_i2c_register_device(adap, node); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
		if (IS_ERR(client)) {
			dev_err(&adap->dev,
				 "Failed to create I2C device for %pOF\n",
				 node);
			of_node_clear_flag(node, OF_POPULATED);
		}
	}

	of_node_put(bus);
}

// drivers/i2c/i2c-core-of.c
static struct i2c_client *of_i2c_register_device(struct i2c_adapter *adap,
						 struct device_node *node)
{
	struct i2c_client *client;
	struct i2c_board_info info;
	int ret;

	dev_dbg(&adap->dev, "of_i2c: register %pOF\n", node);

	ret = of_i2c_get_board_info(&adap->dev, node, &info); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	if (ret)
		return ERR_PTR(ret);

	client = i2c_new_client_device(adap, &info); // 创建i2c_client以便i2c bus match
	if (IS_ERR(client))
		dev_err(&adap->dev, "of_i2c: Failure registering %pOF\n", node);

	return client;
}

int of_i2c_get_board_info(struct device *dev, struct device_node *node,
			  struct i2c_board_info *info)
{
	u32 addr;
	int ret;

	memset(info, 0, sizeof(*info));

	if (of_modalias_node(node, info->type, sizeof(info->type)) < 0) {
		dev_err(dev, "of_i2c: modalias failure on %pOF\n", node);
		return -EINVAL;
	}

	ret = of_property_read_u32(node, "reg", &addr); // <<<<<<<<<<<<<<<<<<< i2c 设备地址
	if (ret) {
		dev_err(dev, "of_i2c: invalid reg on %pOF\n", node);
		return ret;
	}

	if (addr & I2C_TEN_BIT_ADDRESS) {
		addr &= ~I2C_TEN_BIT_ADDRESS;
		info->flags |= I2C_CLIENT_TEN;
	}

	if (addr & I2C_OWN_SLAVE_ADDRESS) {
		addr &= ~I2C_OWN_SLAVE_ADDRESS;
		info->flags |= I2C_CLIENT_SLAVE;
	}

	info->addr = addr;
	info->of_node = node;
	info->fwnode = of_fwnode_handle(node);

	if (of_property_read_bool(node, "host-notify"))
		info->flags |= I2C_CLIENT_HOST_NOTIFY;

	if (of_get_property(node, "wakeup-source", NULL))
		info->flags |= I2C_CLIENT_WAKE;

	return 0;
}
```
