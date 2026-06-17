#include <linux/module.h>
#include <linux/kernel.h>

static int __init my_init(void)
{
    pr_info("My test module loaded!!!!\n");
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("My test module unloaded!!!\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");