#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>


char kernel_buff_poke[9]={0};                        // Ask user to paste data on here
int number_items_in_aray_poke = 0;                   // Number of items currently in array


static int open_sesame(struct inode* inode_pointer, struct file* file_pointer) {
    printk(KERN_INFO "Opened!");
    return 0;
}

static int close_sesame(struct inode* inode_pointer, struct file* file_pointer) {
    printk(KERN_INFO "Closed!");
    return 0;
}

static ssize_t write_sesame(struct file *file, const char *data, size_t length, loff_t *offset_in_file){
    char* address_to_put_data;
    char data_storage;
    int j = 0;
    // For all the bytes the user gives, read byte by byte.
    for(j = 0; j < length; j++){
        long copied = copy_from_user(&kernel_buff_poke[number_items_in_aray_poke], (data + j), 1);
        if (copied != 0){ 
            //handle error
            printk (KERN_INFO "poke: error while copying single bytes from user.");
            return -EFAULT;
        }
        number_items_in_aray_poke++;
        // Make packets to work
        if (number_items_in_aray_poke == 9){
            data_storage = kernel_buff_poke[8];
            //Make into address
            memcpy(&address_to_put_data, kernel_buff_poke, 8);
            *address_to_put_data = data_storage;
            printk (KERN_INFO "poke: the pointer in kernel is blah %p.", address_to_put_data);
            printk (KERN_INFO "poke: the char in kernel is blah %c.", data_storage);
            number_items_in_aray_poke = 0;
        }
    }
    //Success
    return 0;
}

static struct device* device_data_poke;
static struct class* class_stuff_poke;

static struct file_operations file_ops =
{
    .open = open_sesame,
    .release = close_sesame,
    .write = write_sesame,
};

static int __init hi(void) {

    int major = register_chrdev(0, "Poke", &file_ops);   
    class_stuff_poke = class_create(THIS_MODULE, "poke class");
    device_data_poke = device_create(class_stuff_poke, NULL, MKDEV(major, 0), NULL, "poke");
    
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
MODULE_DESCRIPTION("A character device that writes to kernel space!");

