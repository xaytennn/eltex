#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/rwlock.h>

static int major = 0;
static rwlock_t lock;
static char test_string[15] = "Hello\n\0";

ssize_t test_read(struct file *fd, char __user *buffer, size_t size, loff_t *off){
    size_t rc;

    read_lock(&lock);
    rc = simple_read_from_buffer(buffer, size, off, test_string, 15);    
    read_unlock(&lock);
    return rc;
}


ssize_t test_write(struct file *fd, const char __user *buffer, size_t size, loff_t *off){
    size_t rc = 0;
    if(size > 15)
        return -EINVAL;
    
    write_lock(&lock);
    rc = simple_write_to_buffer(test_string, 15, off, buffer, size);
    write_unlock(&lock);

    return rc;

}

static struct file_operations  fops = {
    .owner = THIS_MODULE,
    .read = test_read,
    .write = test_write
};


static int __init my_init(void)
{
    pr_info("My test module loaded!!!!\n");
    rwlock_init(&lock);
    major = register_chrdev(major, "test2", &fops);
    if(major < 0)
        return major;
    pr_info("Major number is %d \n", major);    
    return 0;
}

static void __exit my_exit(void)
{
    pr_info("My test module unloaded!!!\n");
    unregister_chrdev(major, "test2");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");