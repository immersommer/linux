// SPDX-License-Identifier: GPL-2.0
/*
 * The fastcall mechanism allows to register system call handlers 
 * that execute in a minimal kernel environment with reduced overhead.
 */

#include <linux/printk.h>
#include <linux/compiler_types.h>
#include <linux/printk.h>
#include <linux/mm_types.h>
#include <linux/mmap_lock.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <asm/fastcall.h>

/*
 * setup_fastcall_yellow_page
 * - insert a variable to this page
 * - find the correspondent vma,change it to only readable
 */

//Allocating Memory in the Kernel
int fastcall_write(unsigned long __user user_addr, unsigned long len){
	void *address;
	int res;

	address = kmalloc(len, GFP_KERNEL);
	if (!address){
		printk("failure: kmalloc()\n");
	}
  	printk("I got: %zu bytes of memory\n", ksize(address));

	memset(address, 'F', ksize(address));

	res = copy_to_user(user_addr, address, ksize(address));
	printk("wr_res = %d\n", res);

	kfree(address);

	return 0;
}

int fastcall_read(unsigned long __user user_addr, unsigned long len)
{
	void *address;
	int res;

	address = kmalloc(len, GFP_KERNEL);
	if (!address){
		printk("failure: kmalloc()\n");
	}
  	printk("I got: %zu bytes of memory\n", ksize(address));

	memset(user_addr, 'F', len);

	res = copy_from_user(address, user_addr, len);
	printk("rd_res = %d\n", res);

	kfree(address);

	return 0;
} 