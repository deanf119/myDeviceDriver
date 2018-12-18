#include <linux/module.h>
#include  <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>

// For read
char return_data_char;

//For Write
char kernel_buff_peek[8]={0};                        // Ask user to paste data on here
int number_items_in_aray_peek = 0;                   // Number of items currently in array


static int open_sesame(struct inode* inode_pointer, struct file* file_pointer) {
    printk(KERN_INFO "Opened!");
    return 0;
}

static int close_sesame(struct inode* inode_pointer, struct file* file_pointer) {
    printk(KERN_INFO "Closed!");
    return 0;
}

static ssize_t read_sesame(struct file *file, char *data, size_t length, loff_t *offset_in_file){
    
    int retvar;

    // Send data to user
    //retvar = copy_to_user(data, &ptr_return_data_char, 1);
    retvar = copy_to_user(data, &return_data_char, 1);
    if (retvar!=0){
        //handle error
        printk (KERN_INFO "peek: not able to send data to user");
        return -EFAULT;
    }

    //Success
    return 0;
}

static ssize_t write_sesame(struct file *file, const char *data, size_t length, loff_t *offset_in_file){

    char* address_to_put_data;
    int j = 0;
    // For all the bytes the user gives, read byte by byte.
    for(j = 0; j < length; j++){
        long copied = copy_from_user(&kernel_buff_peek[number_items_in_aray_peek], (data + j), 1);
        if (copied != 0){ 
            //handle error
            printk (KERN_INFO "poke: error while copying single bytes from user.");
            return -EFAULT;
        }
        number_items_in_aray_peek++;
        // Make packets to work
        if (number_items_in_aray_peek == 8){
            //Make into address
            memcpy(&address_to_put_data, kernel_buff_peek, 8);
            return_data_char = *address_to_put_data;
            printk (KERN_INFO "peek: the pointer in kernel is blah %p.", address_to_put_data);
            printk (KERN_INFO "peek: the char in kernel is blah %c.", return_data_char);
            number_items_in_aray_peek = 0;
        }
    }
    //Success
    return 0;
}

static struct device* device_data_peek;
static struct class* class_stuff_peek;

static struct file_operations file_ops =
{
  .open = open_sesame,
  .release = close_sesame,
  .read = read_sesame,
  .write = write_sesame,
};

static int __init hi(void) {

  int major = register_chrdev(0, "Peek", &file_ops);   
  class_stuff_peek = class_create(THIS_MODULE, "peek class");
  device_data_peek = device_create(class_stuff_peek, NULL, MKDEV(major, 0), NULL, "peek");
  
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
MODULE_DESCRIPTION("A character device that reads/writes and to/from userspace!");
