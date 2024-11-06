#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#define MAX_BUFFER_SIZE 512

char buffer[MMAX_BUFFER_SIZE];

dev_t device_number; //unique device number associated with the device. (MAJOR : represents a specific type of device and MINOR : represents the instance of the device)
struct cdev pcdev; // represents a specific instance of character device in VFS

struct class *class_pcd; //represents the class of the device & class entry in /sys/class

struct device *device_pcd; //represents the device instance & device entry in /dev



ssize_t pcd_read(struct file* filp,char __user *buff, size_t count, loff_t *f_pos){
    pr_info("Requested to read %d bytes from the device buffer\n", count);
    
    pr_info("Current file position is %d\n", *f_pos);
    
    if((f_pos+count) > MAX_BUFFER_SIZE){ //reading past the device memory - not allowed
        count = MAX_BUFFER_SIZE - *f_pos;
    }
    
    if(copy_to_user(buff, &buffer[*f_pos], count)){
        pr_err("Read is failed\n");
        return -EFAULT;
    }
    
    *f_pos += count;
    
    pr_info("Successfully read %d bytes from device\n", count);
    pr_info("Updated file position is %d\n", *f_pos);
    
    return count;
}


ssize_t pcd_write(struct *file filp, char __user *buff, size_t count, loff_t *f_pos){
    pr_info("Requested to write %d bytes in the device memory\n", count);
    pr_info("Current file position is %d\n", *f_pos);
    
    if((*f_pos + count) > MAX_BUFFER_SIZE){ // device buffer overflow - writing past the device memory size - not allowed
        count = MAX_BUFFER_SIZE - *f_pos;
    }
    
    if(copy_from_user(&buffer[*f_pos], buff, count)){
        pr_err("Write is failed\n");
        return -EFAULT;
    }
    
    *f_pos += count;
    
    pr_info("Successfully wrote %d bytes on to the device\n", count);
    
    return count;
}


int pcd_open(struct inode* inode, struct file* filp){
    pr_info("open was successful\n");
    return 0;
}

int pcd_relese(struct inode *inode, struct file*filp){
    pr_info("release was successful\n");
    return 0;
}

struct file_operations fops= {
    .open = pcd_open,
    .release = pcd_release,
    .read = pcd_read,
    .write = pcd_write,
    .owner = THIS_MODULE
};

int ret_value; //stores the return value of function calls to check the status of the call.

static int __init pseudo_char_init(void){
    /** Device number creation **/
    
    ret_value = alloc_chrdev_region(&device_number, 0, 1, "pcd");
    if(ret_value < 0){
        pr_err("Alloc chrdev failed\n");
        goto out;
        //return ret_value;
    }
    
    /** device registration with the VFS **/
    
    cdev_init(&pcdev, &fops); //pcdev.ops = fops;
    cdev.owner = THIS_MODULE; //This prevents the module from unloading when the cdev structure is still in use 
    
    ret_value = cdev_add(&pcdev, &device_number, 1); // here is where the actual registration happens
    if(ret_value < 0){
        pr_err("Registration with VFS failed\n");
        goto unreg_chrdev_region;
    }
    
    /** creates a device class under /sys/class/ **/
    
    class_pcd = class_create("pcd_class");
    if(IS_ERR(class_pcd)){
        pr_err("Class creation failed");
        goto cdev_del;
    }
    
    /** populates device info in sysfs **/
    
    device_pcd = device_create(class_pcd,NULL,device_number,NULL,"pcd");
	if(IS_ERR(device_pcd)){
		pr_err("Device create failed\n");
		ret = PTR_ERR(device_pcd);
		goto class_del;
	}

	pr_info("Module init was successful\n");
    
    return 0; // returns zero on success
}

class_del:
    class_destroy(class_pcd);

cdev_del:
    cdev_del(&pcdev);

unreg_chrdev_region:
    unregister_chrdev_region(device_number,1);

out:
     pr_info("Module Insertion failed");
     return ret_value;

static void __exit pseudo_char_exit(void){
    device_destroy(class_pcd,device_number);
	class_destroy(class_pcd);
	cdev_del(&pcdev);
	unregister_chrdev_region(device_number,1);
	pr_info("module unloaded\n");
}

module_init(pseudo_char_init);
module_exit(pseudo_char_exit);

MODULE_AUTHOR("Sireesha");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is a pseudo char device driver");