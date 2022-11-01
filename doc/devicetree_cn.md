## 设备树
* [arm64上设备树分析](./arm64_dt.md)

### 参考
[Linux设备树详解(一) 基础知识](https://blog.csdn.net/u012489236/article/details/97137007)  
[Linux设备树详解(四)kernel的解析](https://blog.csdn.net/u012489236/article/details/97271797)  
[arm64 linux device tree加载及调用过程笔记](https://blog.csdn.net/dxjshr/article/details/84935388)  
[设备树处理之——device_node转换成platform_device](https://www.cnblogs.com/downey-blog/p/10486568.html)  
[Linux 驱动开发 十六：设备树在系统中的体现 1](https://blog.csdn.net/OnlyLove_/article/details/121987183)  
[Linux 驱动开发 十六：设备树在系统中的体现 2](https://www.cnblogs.com/multimicro/p/11905647.html)  

## devicetree加载resource流程
of_platform_sync_state_init -> of_platform_default_populate -> of_platform_populate -> of_platform_bus_create -> of_platform_device_create_pdata ->  


of_platform_sync_state_init -> 
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
		/* Populate everything else. */
		of_platform_default_populate(NULL, NULL, NULL);
	}

	return 0;
}
arch_initcall_sync(of_platform_default_populate_init);

```

of_platform_default_populate ->
```c
// drivers/of/platform.c
int of_platform_default_populate(struct device_node *root, const struct of_dev_auxdata *lookup, struct device *parent)
{
	return of_platform_populate(root, of_default_bus_match_table, lookup, parent);
}

```

of_platform_populate -> of_platform_bus_create
```c
// drivers/of/platform.c
int of_platform_populate(struct device_node *root, const struct of_device_id *matches, const struct of_dev_auxdata *lookup, struct device *parent)
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
		rc = of_platform_bus_create(child, matches, lookup, parent, true);
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

of_platform_bus_create -> of_platform_device_create_pdata
```c
// drivers/of/platform.c
static int of_platform_bus_create(struct device_node *bus,
				  const struct of_device_id *matches,
				  const struct of_dev_auxdata *lookup,
				  struct device *parent, bool strict)
{
	const struct of_dev_auxdata *auxdata;
	struct device_node *child;
	struct platform_device *dev;
	const char *bus_id = NULL;
	void *platform_data = NULL;
	int rc = 0;

	...
	dev = of_platform_device_create_pdata(bus, bus_id, platform_data, parent);
	if (!dev || !of_match_node(matches, bus))
		return 0;

	for_each_child_of_node(bus, child) {
		pr_debug("   create child: %pOF\n", child);
		rc = of_platform_bus_create(child, matches, lookup, &dev->dev, strict);
		if (rc) {
			of_node_put(child);
			break;
		}
	}
	of_node_set_flag(bus, OF_POPULATED_BUS);
	return rc;
}
```
of_platform_device_create_pdata -> of_device_alloc
```c
// drivers/of/platform.c
struct platform_device *of_device_alloc(struct device_node *np, const char *bus_id, struct device *parent)
{
	struct platform_device *dev;
	int rc, i, num_reg = 0;
	struct resource *res, temp_res;

	dev = platform_device_alloc("", PLATFORM_DEVID_NONE);
	if (!dev)
		return NULL;

	/* count the io resources */
	while (of_address_to_resource(np, num_reg, &temp_res) == 0)
		num_reg++;

	/* Populate the resource table */
	if (num_reg) {
		res = kcalloc(num_reg, sizeof(*res), GFP_KERNEL);
		if (!res) {
			platform_device_put(dev);
			return NULL;
		}

		dev->num_resources = num_reg;
		dev->resource = res;
		for (i = 0; i < num_reg; i++, res++) {
			rc = of_address_to_resource(np, i, res);
			WARN_ON(rc);
		}
	}

	dev->dev.of_node = of_node_get(np);
	dev->dev.fwnode = &np->fwnode;
	dev->dev.parent = parent ? : &platform_bus;

	if (bus_id)
		dev_set_name(&dev->dev, "%s", bus_id);
	else
		of_device_make_bus_id(&dev->dev);

	return dev;
}
```
of_address_to_resource
```c
// drivers/of/address.c
int of_address_to_resource(struct device_node *dev, int index,
			   struct resource *r)
{
	return __of_address_to_resource(dev, index, -1, r);
}

static int __of_address_to_resource(struct device_node *dev, int index, int bar_no,
		struct resource *r)
{
	u64 taddr;
	const __be32	*addrp;
	u64		size;
	unsigned int	flags;
	const char	*name = NULL;

	addrp = __of_get_address(dev, index, bar_no, &size, &flags);
	if (addrp == NULL)
		return -EINVAL;

	/* Get optional "reg-names" property to add a name to a resource */
	if (index >= 0)
		of_property_read_string_index(dev, "reg-names",	index, &name);

	if (flags & IORESOURCE_MEM)
		taddr = of_translate_address(dev, addrp);
	else if (flags & IORESOURCE_IO)
		taddr = of_translate_ioport(dev, addrp, size);
	else
		return -EINVAL;

	if (taddr == OF_BAD_ADDR)
		return -EINVAL;
	memset(r, 0, sizeof(struct resource));

	if (of_mmio_is_nonposted(dev))
		flags |= IORESOURCE_MEM_NONPOSTED;

	r->start = taddr;
	r->end = taddr + size - 1;
	r->flags = flags;
	r->name = name ? name : dev->full_name;

	return 0;
}
```












