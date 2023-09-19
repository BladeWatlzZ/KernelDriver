#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>    // __init、__exit
#include <linux/kernel.h>  // printk、KERN_INFO
#include <linux/module.h>  // all module need.

#define DEVICE_NAME "regctl"
#define DEVICE_CNT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lzp");
MODULE_DESCRIPTION("Control Fpga registers.");

static int dev_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "regctl: regctl_open\n");
    return 0;
}

static int dev_release(struct inode* inode, struct file* file) {
    printk(KERN_INFO "regctl: regctl_release\n");
    return 0;
}

static ssize_t dev_read(struct file* file,
                        char __user* buffer,
                        size_t       length,
                        loff_t*      offset) {
    printk(KERN_INFO "regctl: regctl_read\n");
    return 0;
}

static ssize_t dev_write(struct file*       file,
                         const char __user* buffer,
                         size_t             length,
                         loff_t*            offset) {
    printk(KERN_INFO "regctl: regctl_write\n");
    return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static dev_t dev_id;
static unsigned int
    major_num;  // major dev num, if not specified, auto allocated
static struct cdev    my_cdev;
static struct class*  my_class;
static struct device* my_device;

static int __init regctl_init(void) {
    int ret;
    if (major_num) {
        dev_id = MKDEV(major_num, 0);
        ret = register_chrdev_region(dev_id, DEVICE_CNT, DEVICE_NAME);
    } else {
        ret = alloc_chrdev_region(&dev_id, 0, DEVICE_CNT, DEVICE_NAME);
        major_num = MAJOR(dev_id);
    }
    if (ret < 0) {
        printk(KERN_ALERT "regctl: Failed to register character device.\n");
        return ret;
    }
    // init character device struct.
    cdev_init(&my_cdev, &fops);

    // add character device struct to linux kernel.
    if (cdev_add(&my_cdev, dev_id, DEVICE_CNT) == -1) {
        unregister_chrdev_region(dev_id, DEVICE_CNT);
        printk(KERN_ALERT "regctl: Failed to add character device.\n");
        return -1;
    }

    // create device class
    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_id, DEVICE_CNT);
        printk(KERN_ALERT "regctl: Failed to create device class.\n");
        return -1;
    }

    // create device file
    my_device = device_create(my_class, NULL, dev_id, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_id, DEVICE_CNT);
        printk(KERN_ALERT "regctl: Failed to create device file.\n");
        return -1;
    }

    printk(KERN_INFO
           "regctl: Character device registered : %s, major_num = %d\n",
           DEVICE_NAME, major_num);
    printk(KERN_INFO "regctl: Init.\n");
    return 0;
}

static void __exit regctl_exit(void) {
    // delete device file
    device_destroy(my_class, dev_id);

    // delete device class
    class_destroy(my_class);

    // delete the character device from kernel.
    cdev_del(&my_cdev);

    // unregister the device number.
    unregister_chrdev_region(dev_id, DEVICE_CNT);

    printk(KERN_INFO "regctl: Exit\n");
}

module_init(regctl_init);
module_exit(regctl_exit);