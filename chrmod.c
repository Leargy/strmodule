#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/debugfs.h>


#include <linux/sched.h>
#include <linux/pid.h>

#define KKK

#include "chrmod.h"

#define MYDEV_NAME "chardev"
#define KBUF_SIZE (size_t) ((10) * PAGE_SIZE)

static dev_t first_dev;
static int my_major = 100, my_minor = 0;
static struct cdev * my_cdev;

static struct task_struct *t_str;
static struct thread_struct thr_str = {.iopl_emul = 2};
static struct page pg_str;

static struct dentry *dir;
static unsigned int PID = 0;

static int mychrdev_open(struct inode *inode, struct file *file) {
    static int counter = 0;

    char *kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
    file->private_data = kbuf;

    printk(KERN_INFO "Open device %s:\n\n", MYDEV_NAME);
    counter++;

    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Release device %s:\n\n", MYDEV_NAME); 

    return 0;
}

static long mychrdev_ioctl( struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    printk(KERN_INFO "Ioctl device %s:\n\n", MYDEV_NAME); 
    
    void __user *arg_user;
    arg_user = (void __user*) ioctl_param;

    switch(ioctl_num) {
	case THREAD_STRUCT_IOCTL_SET_PID:
            copy_from_user(&PID, arg_user, sizeof(unsigned int));
	    break;
	case THREAD_STRUCT_IOCTL_GET:
	    printk(KERN_INFO "EXTRUCTING THREAD STRUCTURE:");
	    
	    t_str = get_pid_task(find_get_pid(1), PIDTYPE_PID);
            thr_str = t_str->thread;

	    copy_to_user(arg_user, &thr_str, sizeof(struct thread_struct)); 
	    break;
	case PAGE_IOCTL_GET:
            printk(KERN_INFO "EXECUTING PAGE STRUCTURE:");
            
            pg_str = *(t_str->task_frag.page);
            
	    copy_to_user(arg_user, &pg_str, sizeof(struct page));
            
	    break;
    }

    return 0;
}

static const struct file_operations mycdev_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = mychrdev_ioctl,
    .open = mychrdev_open,
    .release = mychrdev_release
};

static int __init chrdev_init(void) {
    printk(KERN_INFO "thrmod: module loading.\n");
    
    dir = debugfs_create_dir("kkkk_ioctl", 0);
    debugfs_create_file("ng", 0, dir, NULL, &mycdev_fops);
    
    return 0;
}

static void __exit chrdev_exit(void) {
    printk(KERN_INFO "thrmod: module unloaded.\n");
    
    debugfs_remove_recursive(dir);
}

MODULE_LICENSE("Dual BSD/GPL");

module_init(chrdev_init);
module_exit(chrdev_exit);
