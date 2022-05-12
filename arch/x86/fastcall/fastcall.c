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
#include <asm/fastcall.h>

#define NR_REQ 1
/*
 * setup_fastcall_yellow_page
 * - insert a variable to this page
 * - find the correspondent vma,change it to only readable
 */
int fastcall_write(unsigned long __user user_addr, unsigned long len)
{
	struct page *pages[NR_REQ];
	int Nr_page = len/PAGE_SIZE;
	int nr_gup;
	void *address;
    int ret;

    ret = 0;
	if (Nr_page != NR_REQ) {
		pr_info("fastcall_write func: provided len argument invalid,must be page size");
		return -EINVAL;
	}
	pr_info("fastcall register func: fastcall_write func begin");
    // pin the page to memory and get struct page
	mmap_read_lock(current->mm);
	nr_gup = get_user_pages(user_addr, NR_REQ, FOLL_WRITE, pages, NULL);
    if (nr_gup < 0) {
		pr_info("fastcall_write func: get_user_pages return: %d\n", nr_gup);
		ret = nr_gup;
		goto fail_get_user_page;
    }
	mmap_read_unlock(current->mm);
    // map the page to kernel using kmap and get the correspondent kernel vir. addr. of this page
	address = kmap(pages[0]);
    // write a int value at the begining of this page
	memset(address, 'F', PAGE_SIZE);
	pr_info("fastcall register func: set F to the maped page");
    // kunmap from kernel
	kunmap(pages[0]);
    /*Pages pinned by get_user_pages() need to be 'put' using put_page() 
	  when finished with. If they have been written to, 
	  they first need to be marked 'dirty' using set_page_dirty_lock(). */
	set_page_dirty_lock(pages[0]);
	put_page(pages[0]);
	pr_info("fastcall register func: fastcall_write func end");
	return 0;

fail_get_user_page:
		mmap_read_unlock(current->mm);
	return ret;
} 

int fastcall_read(unsigned long __user user_addr, unsigned long len)
{
	struct page *pages[NR_REQ];
	int Nr_page = len/PAGE_SIZE;
	int nr_gup;
	void *address;
    int ret;

    ret = 0;
	if (Nr_page != NR_REQ) {
		pr_info("fastcall_read func: provided len argument invalid,must be page size");
		return -EINVAL;
	}
	pr_info("fastcall register func: fastcall_read func begin");
    // pin the page to memory and get struct page
	mmap_read_lock(current->mm);
	nr_gup = get_user_pages(user_addr, NR_REQ, FOLL_WRITE, pages, NULL);
    if (nr_gup < 0) {
		pr_info("fastcall_read func: get_user_pages return: %d\n", nr_gup);
		ret = nr_gup;
		goto fail_get_user_page;
    }
	mmap_read_unlock(current->mm);
    // map the page to kernel using kmap and get the correspondent kernel vir. addr. of this page
	address = kmap(pages[0]);
    // copy characters from kernel vir. addr. to user address
	memcpy(nr_gup, address, PAGE_SIZE);
	pr_info("fastcall register func: set F to the maped page");
    // kunmap from kernel
	kunmap(pages[0]);
    /*Pages pinned by get_user_pages() need to be 'put' using put_page() 
	  when finished with. If they have been written to, 
	  they first need to be marked 'dirty' using set_page_dirty_lock(). */
	set_page_dirty_lock(pages[0]);
	put_page(pages[0]);
	pr_info("fastcall register func: fastcall_read func end");
	return 0;

fail_get_user_page:
	mmap_read_unlock(current->mm);
	return ret;
} 