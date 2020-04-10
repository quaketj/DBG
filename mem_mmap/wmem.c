#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <linux/device.h>
#include <linux/err.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "wmem.h"

#define MODULE_NAME "wmem"

static int wmem_open (struct inode *inode, struct file *file) {
	return 0;
}

static int wmem_release (struct inode *inode, struct file *file) {
	return 0;
}

static int wmem_mmap(struct file *fp, struct vm_area_struct *vma) {
	int ret;
	int start_pfn = vma->vm_pgoff;
	int end_pfn = start_pfn + ((vma->vm_end - vma->vm_start) >> PAGE_SHIFT);
	int i;

    if (start_pfn == 0x83fffe10)
        vma->vm_flags |= VM_IO;

	for (i = start_pfn; i < end_pfn; i++) {
		if ((ret = remap_pfn_range(vma,
						vma->vm_start + (i - start_pfn) * PAGE_SIZE,
						i,
						PAGE_SIZE,
						vma->vm_page_prot)) < 0) {
			printk("remap_pfn_range fail at pfn %x, ret %d.\n", i, ret);
			return ret;
		}
	}

	return 0;
}

static long wmem_unlocked_ioctl(struct file *file,
		unsigned int cmd, unsigned long arg) {
	int retval = 0;

	switch ( cmd ) {
		case FLUSH_CACHE:
			wbinvd();
			break;
		default:
			retval = -EINVAL;
	}
	return retval;
}

struct file_operations wmem_fops = {
	.owner		    =	THIS_MODULE,
	.unlocked_ioctl	=	wmem_unlocked_ioctl,
	.mmap		    =	wmem_mmap,
	.open		    =	wmem_open,
	.release	    =	wmem_release,
};

#define WMEM_MAX_DEVS	1
static struct cdev wmem_cdev; 
static struct class *wmem_class; 
static int wmem_major;

static int __init wmem_init_module (void) {
	int ret;
	dev_t devid;

	wmem_class = class_create(THIS_MODULE, MODULE_NAME);
	if (IS_ERR(wmem_class)) {
		ret = PTR_ERR(wmem_class);
		goto out;
	}

	ret = alloc_chrdev_region(&devid, 0, WMEM_MAX_DEVS, MODULE_NAME);
	if (ret) {
		printk("%s, alloc_chrdev_region failed.\n", __FUNCTION__);
		goto destroy_wmem_class;
	}

	wmem_major = MAJOR(devid);

	if (wmem_class) {
		struct device *dev = device_create(wmem_class,
				NULL, MKDEV(wmem_major, 0), NULL,
				MODULE_NAME);
		if (IS_ERR(dev)) {
			printk("%s, device_create failed.\n", __FUNCTION__);
			goto unregister_chrdev;
		}
	}

	cdev_init(&wmem_cdev, &wmem_fops);
	ret = cdev_add(&wmem_cdev, MKDEV(wmem_major, 0), WMEM_MAX_DEVS);
	if (ret)
		goto unregister_chrdev;

	return 0;

unregister_chrdev:
	unregister_chrdev_region(MKDEV(wmem_major, 0), WMEM_MAX_DEVS);

destroy_wmem_class:
	class_destroy(wmem_class);

out:
	return ret;
}

static void __exit wmem_cleanup_module (void) {
	if (wmem_class) {
		device_destroy(wmem_class, MKDEV(wmem_major, 0));
		class_destroy(wmem_class);
    }

	if (wmem_major)
		unregister_chrdev_region(MKDEV(wmem_major, 0), WMEM_MAX_DEVS);
	
	cdev_del(&wmem_cdev);
}

module_init(wmem_init_module);
module_exit(wmem_cleanup_module);

MODULE_AUTHOR("QUAKE WANG");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Memory mapping helper module.");

