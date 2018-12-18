#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>

char *ptr_device_buffer;        // For global use of kernel buffer space.

static int open_sesame(struct inode* inode_pointer, struct file* file_pointer) {
    printk(KERN_INFO "Opened!");
    ptr_device_buffer = kmalloc(1024, GFP_KERNEL);
    printk (KERN_INFO "getptr: the pointer in kernel is %p.", ptr_device_buffer);
    return 0;
}

static int close_sesame(struct inode* inode_pointer, struct file* file_pointer) {
    printk(KERN_INFO "Closed!");
    return 0;
}

static ssize_t read_sesame(struct file *file, char *data, size_t length, loff_t *offset_in_file){

    int retvar = 0;
    if (!ptr_device_buffer){
        //handle error
        printk (KERN_INFO "getptr: not able to allocate 1024 bytes of memory");
        return -EFAULT;
    }

    printk (KERN_INFO "getptr: reading from device");

    // Send address to user
    retvar = copy_to_user(data, &ptr_device_buffer, 8);
    if (retvar!=0){
        //handle error
        printk (KERN_INFO "getptr: not able to send address to user");
        return -EFAULT;
    }

    printk (KERN_INFO "getptr: the pointer in kernel is %p.", ptr_device_buffer);


    //Success
    return 0;
}


static struct device* device_data_getptr;
static struct class* class_stuff_getptr;

static struct file_operations file_ops =
{
    .open = open_sesame,
    .release = close_sesame,
    .read = read_sesame,
};

static int __init hi(void) {

    int major = register_chrdev(0, "Getptr", &file_ops);   
    class_stuff_getptr = class_create(THIS_MODULE, "getptr class");
    device_data_getptr = device_create(class_stuff_getptr, NULL, MKDEV(major, 0), NULL, "getptr");
    
    printk(KERN_INFO "HI!\n");
    return 0;
}

static void __exit bye(void) {
    printk(KERN_INFO "BYE!\n");
}

module_init(hi);
module_exit(bye);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("DS");
MODULE_DESCRIPTION("A character device driver that sends a pointer for the freespace to userspace!");

