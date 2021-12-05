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
#include <linux/mm.h>

#define KKK

#include "chrmod.h"

#define MYDEV_NAME "chardev"

static int my_major = 100, my_minor = 0;

static struct task_struct *t_str;
static struct thread_struct thr_str = {.iopl_emul = 2};
static struct page pg_str;

static struct dentry *dir;
static unsigned int PID = 0;

static int mychrdev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Open device %s:\n\n", MYDEV_NAME);

    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Release device %s:\n\n", MYDEV_NAME); 

    return 0;
}

static long mychrdev_ioctl( struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
    printk(KERN_INFO "Ioctl device %s:\n", MYDEV_NAME); 
    
    void __user *arg_user;
    arg_user = (void __user*) ioctl_param;

    switch(ioctl_num) {
	case THREAD_STRUCT_IOCTL_SET_PID:
            copy_from_user(&PID, arg_user, sizeof(unsigned int));
            printk(KERN_INFO "GOT PID-%d FROM USER\n\n", PID);
	    break;
	case THREAD_STRUCT_IOCTL_GET:
	    printk(KERN_INFO "EXTRUCTING THREAD STRUCTURE FROM PID-%d\n\n", PID);
	    
	    t_str = get_pid_task(find_get_pid(PID), PIDTYPE_PID);
            printk(KERN_INFO "T_s - %p\n", t_str);
              
            
	    if(t_str == NULL) {
	        thr_str = (struct thread_struct){};
	    }else {
	        thr_str = t_str->thread;
	    }
           
	    copy_to_user(arg_user, &thr_str, sizeof(struct thread_struct)); 
	    break;
	case PAGE_IOCTL_GET:
            printk(KERN_INFO "EXTRUCTING PAGE STRUCTURE\n\n");
            
            //pg_str = *(node_data[1]->node_mem_map);
            //pg_str = *(t_str->task_frag.page);
            get_page(&pg_str);


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
    printk(KERN_INFO "chrmod: module loading.\n");
    
    dir = debugfs_create_dir("kkkk_ioctl", 0);
    debugfs_create_file("ng", 0, dir, NULL, &mycdev_fops);
    
    return 0;
}

static void __exit chrdev_exit(void) {
    printk(KERN_INFO "chrmod: module unloaded.\n");
    
    debugfs_remove_recursive(dir);
}

MODULE_LICENSE("Dual BSD/GPL");

module_init(chrdev_init);
module_exit(chrdev_exit);
